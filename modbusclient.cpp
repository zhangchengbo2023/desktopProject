#include "modbusclient.h"
#include <QVariant>
#include <QDebug>
#include "QsLog/QsLog.h"
#include <QModbusDataUnit>

Modbusclient::Modbusclient(QObject *parent) : QObject(parent)
    ,modBusDevice(nullptr)
{
    modBusDevice = new QModbusTcpClient(this);
    connect(modBusDevice,&QModbusTcpClient::stateChanged,this,&Modbusclient::onStateChanged);
    connect(modBusDevice,&QModbusTcpClient::errorOccurred,this,&Modbusclient::onErrorOccurred);

    //有数据 定时发送
    // 初始化定时器
    //timer = new QTimer(this);
    //connect(timer, &QTimer::timeout, this, &Modbusclient::processQueue);
    //timer->start(1000);  // 每1秒定时发送数据
}

void Modbusclient::connectToServer(const QString &addr, int port)
{

    if(!modBusDevice)
        return;
    modBusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter,QVariant::fromValue(addr));
    modBusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter,QVariant::fromValue(port));
    modBusDevice->setTimeout(2000);
    //modBusDevice->setNumberOfRetries(3);

    if(modBusDevice->connectDevice()){
        QLOG_DEBUG()<<"modbus device connected success on ip :"<<addr<<",port:"<<port;
    }else{
        QLOG_DEBUG()<<"modbus device connect ip: "<<  addr <<",failed:"<<modBusDevice->errorString();
    }
}
void Modbusclient::floatToRegisters(float value, uint16_t &highRegister, uint16_t &lowRegister) {
    union {
        float floatValue;
        uint32_t intValue;
    } data;

    data.floatValue = value;
    highRegister = static_cast<uint16_t>((data.intValue >> 16) & 0xFFFF); // 高 16 位

    lowRegister = static_cast<uint16_t>(data.intValue & 0xFFFF);           // 低 16 位

    //QLOG_DEBUG()<<"highRegister";
    //QLOG_DEBUG()<<highRegister;
    //QLOG_DEBUG()<<"lowRegister";
    //QLOG_DEBUG()<<lowRegister;
    float testData = registersToFloat(highRegister,lowRegister);
    //QLOG_DEBUG()<<"true num:";
    QLOG_DEBUG()<<testData;

}
void Modbusclient::float_to_registers(float f, uint16_t* registers) {
    uint32_t* p = reinterpret_cast<uint32_t*>(&f);
    registers[0] = static_cast<uint16_t>((*p & 0xFFFF0000) >> 16);
    registers[1] = static_cast<uint16_t>(*p & 0x0000FFFF);
}

float Modbusclient::registersToFloat(uint16_t highRegister, uint16_t lowRegister) {
    union {
        float floatValue;
        uint32_t intValue;
    } data;

    // 合并高 16 位和低 16 位
    data.intValue = (static_cast<uint32_t>(highRegister) << 16) | static_cast<uint32_t>(lowRegister);
    return data.floatValue;
//    QLOG_DEBUG()<<"true num:";
//    QLOG_DEBUG()<<data.floatValue;
}
void Modbusclient::readRegisters(int startAddr, int numberOfRegisters)
{

    bool status = getUpdateStatus();

    if(status){
        if((!modBusDevice )|| (modBusDevice->state() != QModbusDevice::ConnectedState)){
            QLOG_DEBUG()<<"zero is set.quit reading";
            return;
        }
        //QLOG_DEBUG()<<"Modbusclient::readRegisters,addr:"<<startAddr<<",number:"<<numberOfRegisters;
        QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddr,numberOfRegisters);
        if (auto *reply = modBusDevice->sendReadRequest(readUnit, 3)) {  // 设备ID
                    if (!reply->isFinished()) {
                        connect(reply, &QModbusReply::finished, this, [this, reply]() {
                            if (reply->error() == QModbusDevice::NoError) {
                                const QModbusDataUnit unit = reply->result();
                                for (uint i = 0; i < unit.valueCount(); i++) {
                                    //QLOG_DEBUG() << "Register" << unit.startAddress() + i << "=" << unit.value(i);
                                    if(unit.value(i) > 0){
                                        //寄存器为0 不做处理
                                        emit readValue(unit.startAddress() + i,unit.value(i));
                                    }

                                }
                            } else {
                                QLOG_DEBUG() << "Read response error:" << reply->errorString();
                            }
                            reply->deleteLater();
                        });
                    } else {
                        delete reply;
                    }
                } else {
                    QLOG_DEBUG() << "Read error:" << modBusDevice->errorString();
        }
    }

}
void Modbusclient::writeFloatRegister(int address,float value){

        if (modBusDevice->state() != QModbusDevice::ConnectedState)
            return ;
        float data = value;
        uint16_t registers[2];
        float_to_registers(data, registers);
        //QLOG_DEBUG()<<address;
        //QLOG_DEBUG()<<registers[0];
        //QLOG_DEBUG()<<address+1;
        //QLOG_DEBUG()<<registers[1];
        QModbusDataUnit writeUnit1(QModbusDataUnit::HoldingRegisters, address, 1);
        QModbusDataUnit writeUnit2(QModbusDataUnit::HoldingRegisters, address+1, 1);
        writeUnit1.setValue(0, registers[1]); // 起始地址写入高 16 位
        writeUnit2.setValue(0, registers[0]);  // 下一个地址写入低 16 位
        if (auto *reply = modBusDevice->sendWriteRequest(writeUnit1, 2)) {  // 设备ID
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [reply]() {
                    if (reply->error() == QModbusDevice::NoError) {
                        QLOG_DEBUG() << "Write addr 1st successful!";
                    } else {
                        QLOG_DEBUG() << "Write response addr 1st error:" << reply->errorString();
                    }
                    reply->deleteLater();
                });
            } else {
                delete reply;
            }
        } else {
            QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
        }

        if (auto *reply = modBusDevice->sendWriteRequest(writeUnit2, 2)) {  // 设备ID
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [reply]() {
                    if (reply->error() == QModbusDevice::NoError) {
                        QLOG_DEBUG() << "Write addr 2th successful!";
                    } else {
                        QLOG_DEBUG() << "Write response 2th error:" << reply->errorString();
                    }
                    reply->deleteLater();
                });
            } else {
                delete reply;
            }
        } else {
            QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
        }

}

void Modbusclient::writeIntRegister(int address, uint16_t value)
{

    if (modBusDevice->state() != QModbusDevice::ConnectedState)
        return ;
    //float data = value;
    uint16_t registers[2];
    registers[0] = value;
    registers[1] = 0;
    //float_to_registers(data, registers);
    QLOG_DEBUG()<<"address write:"<<address;
    QLOG_DEBUG()<<"value"<<value;

    QModbusDataUnit writeUnit1(QModbusDataUnit::HoldingRegisters, address, 1);
    QModbusDataUnit writeUnit2(QModbusDataUnit::HoldingRegisters, address+1, 1);
    writeUnit1.setValue(0, registers[0]); //第一个寄存器
    writeUnit2.setValue(0, registers[1]);//第二个寄存器
    if (auto *reply = modBusDevice->sendWriteRequest(writeUnit1, 2)) {  // 设备ID
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    QLOG_DEBUG() << "Write single int 1st successful!";
                } else {
                    QLOG_DEBUG() << "Write single int 1st error:" << reply->errorString();
                }
                reply->deleteLater();
            });
        } else {
            delete reply;
        }
    } else {
        QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
    }

    if (auto *reply = modBusDevice->sendWriteRequest(writeUnit2, 2)) {  // 设备ID
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    QLOG_DEBUG() << "Write single 2th successful!";
                } else {
                    QLOG_DEBUG() << "Write single 2th error:" << reply->errorString();
                }
                reply->deleteLater();
            });
        } else {
            delete reply;
        }
    } else {
        QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
    }

}

void Modbusclient::writeMultiPleFloatRegister(int address, QVector<float> &values)
{
  QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, address, values.size());
    uint16_t registers[2];
    int flag = values[values.size()-1];
    values.pop_back();
    for (int i = 0; i < values.size(); i+=2) {

        float_to_registers(values[i], registers);
        writeUnit.setValue(i, registers[1]);

        writeUnit.setValue(i+1, registers[0]);


    }

    //最后一个数字 写入3112  3113
    registers[0] = flag;
    registers[1] = 0;
    writeUnit.setValue(values.size(), registers[0]);
    writeUnit.setValue(values.size()+1, registers[1]);
    // 批量写入寄存器
    if (auto *reply = modBusDevice->sendWriteRequest(writeUnit, address)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    QLOG_DEBUG() << "Write multiple float successful!";
                } else {
                    QLOG_DEBUG() << "Write multiple float failed,error:" << reply->errorString();
                }
                reply->deleteLater();
            });
        } else {
            // 如果请求失败
            reply->deleteLater();
        }
    } else {
        QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
    }

}

void Modbusclient::writeMultiPleIntRegister(int address, QVector<int> &values)
{
    if (modBusDevice->state() != QModbusDevice::ConnectedState)
        return ;

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, address, values.size());
    //uint16_t registers[2];
    for (int i = 0; i < values.size(); i+=2) {

        QLOG_DEBUG()<<"write multiple index:"<<i<<",value"<<values[i];
        //registers[0] = values[i];
        //registers[1] = 0;
        writeUnit.setValue(i, values[i]);
        //writeUnit.setValue(i+1, registers[1]);
        //QLOG_DEBUG()<<"multiple int index:"<<i<<",value:"<<values[i];
        //QLOG_DEBUG()<<"int departure 1st:"<<registers[0]<<",2nd :"<<registers[1];
    }
    // 批量写入寄存器
    if (auto *reply = modBusDevice->sendWriteRequest(writeUnit, address)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    QLOG_DEBUG() << "Write multiple int successful!";
                } else {
                    QLOG_DEBUG() << "Write multiple int failed,error:" << reply->errorString();
                }
                reply->deleteLater();
            });
        } else {
            // 如果请求失败
            reply->deleteLater();
        }
    } else {
        QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
    }
}

void Modbusclient::writeMultiPleSingleIntRegister(int address, QVector<int> &values)
{
    if (modBusDevice->state() != QModbusDevice::ConnectedState)
        return ;

    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, address, values.size());
    //uint16_t registers[2];
    for (int i = 0; i < values.size(); i+=2) {

        QLOG_DEBUG()<<"write multiple index:"<<i<<",value"<<values[i];
        //registers[0] = values[i];
        //registers[1] = 0;
        writeUnit.setValue(i, values[i]);
       //writeUnit.setValue(i+1, registers[1]);

    }
    // 批量写入寄存器
    if (auto *reply = modBusDevice->sendWriteRequest(writeUnit, address)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [reply]() {
                if (reply->error() == QModbusDevice::NoError) {
                    QLOG_DEBUG() << "Write multiple int successful!";
                } else {
                    QLOG_DEBUG() << "Write multiple int failed,error:" << reply->errorString();
                }
                reply->deleteLater();
            });
        } else {
            // 如果请求失败
            reply->deleteLater();
        }
    } else {
        QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
    }
}
void Modbusclient::writeRegister(int address, int value)
{
    if (modBusDevice->state() != QModbusDevice::ConnectedState)
            return;


      QLOG_DEBUG()<<"Modbusclient::writeRegister,addr:"<<address<<",value:"<<value;
      QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, address, 1);
      writeUnit.setValue(0, value);


            //int32_t val = value;

            //writeUnit.setValue(0, val);
            //16高位 和16低位 写入2个寄存器
            //writeUnit.setValue(0, lowRegister); // 起始地址写入高 16 位
            //writeUnit.setValue(1, highRegister);  // 下一个地址写入低 16 位

            //整数部分和小数部分 写人2个寄存器
            // 分离整数和小数部分
//            int integerPart = static_cast<int>(std::floor(value)); // 取整数部分
//            int fractionalPart = static_cast<int>((value - integerPart) * 1000); // 小数部分放大1000倍
//            writeUnit.setValue(0,static_cast<int16_t>(integerPart));//整数部分写入第一个寄存器
//            writeUnit.setValue(1,static_cast<int16_t>(fractionalPart));//小数部分写入第二个寄存器
//            QLOG_DEBUG()<<"3100整数部分";
//            QLOG_DEBUG()<<integerPart;
//            QLOG_DEBUG()<<"3101小数部分";
//            QLOG_DEBUG()<<fractionalPart;
            if (auto *reply = modBusDevice->sendWriteRequest(writeUnit, 2)) {  // 设备ID
                if (!reply->isFinished()) {
                    connect(reply, &QModbusReply::finished, this, [this,reply]() {
                        if (reply->error() == QModbusDevice::NoError) {
                            QLOG_DEBUG() << "Write  successful!";
                            setUpdateStatus(true);
                            //emit setZero();
                        } else {
                            QLOG_DEBUG() << "Write response error:" << reply->errorString();
                        }
                        reply->deleteLater();
                    });
                } else {
                    delete reply;
                }
            } else {
                QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
            }

}
void Modbusclient::writeMultipleRegistersWithGap(const QVector<quint16> &addresses, const QVector<quint16> &values) {
    if (!modBusDevice || modBusDevice->state() != QModbusDevice::ConnectedState)
                return;

//    if (addresses.size() != values.size()) {
//        qWarning() << "Address and value arrays must be of the same size!";
//        return;
//    }

    for (int i = 0; i < addresses.size(); ++i) {
        // 每次写入单个寄存器
        QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, addresses[i], 1);
        writeUnit.setValue(0, values[i]);


        if (auto *reply = modBusDevice->sendWriteRequest(writeUnit, 2)) {  // 设备ID
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [reply]() {
                    if (reply->error() == QModbusDevice::NoError) {
                        QLOG_DEBUG() << "Write successful!";
                    } else {
                        QLOG_DEBUG() << "Write response error:" << reply->errorString();
                    }
                    reply->deleteLater();
                });
            } else {
                delete reply;
            }
        } else {
            QLOG_DEBUG() << "Write error:" << modBusDevice->errorString();
        }
    }
}
void Modbusclient::onWriteFinished(QModbusReply* reply)
{
    if (reply->error() == QModbusDevice::NoError) {
        QLOG_DEBUG() << "Write operation successful!";
    } else {
        QLOG_DEBUG() << "Write operation failed with error:" << reply->errorString();
    }

    reply->deleteLater();  // 释放 reply 对象
}
void Modbusclient::onStateChanged(int state)
{
    if (state == QModbusDevice::ConnectedState) {
                QLOG_DEBUG() << "Connected to Modbus server";
            } else if (state == QModbusDevice::UnconnectedState) {
                QLOG_DEBUG() << "Disconnected from Modbus server";
    }
}

void Modbusclient::onErrorOccurred(QModbusDevice::Error error)
{
    if (error == QModbusDevice::NoError)
                return;
            QLOG_DEBUG() << "Error occurred:" << modBusDevice->errorString();
}
