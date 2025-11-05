#include "serialport.h"
#include "QsLog/QsLog.h"
Serialport::Serialport(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort(this);

    serialPort->setBaudRate(QSerialPort::Baud38400);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

}

void Serialport::setPortName(QString portName)
{
    serialPort->setPortName(portName);
}

bool Serialport::openSerialPort()
{
    if(serialPort){
        if(serialPort->open(QIODevice::ReadWrite)){
            QLOG_DEBUG()<<"open port:"<<serialPort->portName();
            return true;
        }else{
            QLOG_DEBUG()<<"open port failed.";
            return false;
        }
    }
}

void Serialport::closeSerialPort()
{
    if(serialPort->isOpen()){
        serialPort->close();
        QLOG_DEBUG()<<"closed port";
    }
}

unsigned short Serialport::calculateCRC(const QByteArray &data)
{
    //qDebug()<<data;
    uint16_t crc = 0xFFFF; // 初始值
       for (uint8_t byte : data) {
           crc ^= byte; // 将当前字节与 CRC 异或
           for (int bit = 0; bit < 8; ++bit) {
               if (crc & 0x0001) {
                   crc = (crc >> 1) ^ 0xA001; // 多项式
               } else {
                   crc >>= 1;
               }
           }
       }
//       uint8_t lowByte = crc & 0xFF;         // 低字节
//       uint8_t highByte = (crc >> 8) & 0xFF; // 高字节
//       qDebug() << "CRC16 Low Byte:" << QString::number(lowByte, 16).toUpper().rightJustified(4, '0');
//       qDebug() << "CRC16 High Byte:" << QString::number(highByte, 16).toUpper().rightJustified(4, '0');
       return crc;

}

QByteArray Serialport::buildReadRequest(quint8 deviceAddr, quint16 registerAddr, quint16 registerCount)
{
    QByteArray request;
    request.append(deviceAddr);
    request.append(0x03);
    request.append(static_cast<char>(registerAddr >> 8));  // 寄存器地址高字节
    request.append(static_cast<char>(registerAddr & 0xFF));  // 寄存器地址低字节
    request.append(static_cast<char>(registerCount >> 8));  // 读取寄存器个数高字节
    request.append(static_cast<char>(registerCount & 0xFF));  // 读取寄存器个数低字节


    quint16 crc = calculateCRC(request);
    uint8_t lowByte = crc & 0xFF;         // 低字节
    uint8_t highByte = (crc >> 8) & 0xFF; // 高字节
    //qDebug() << "CRC16 Low Byte:" << QString::number(lowByte, 16).toUpper().rightJustified(4, '0');
    //qDebug() << "CRC16 High Byte:" << QString::number(highByte, 16).toUpper().rightJustified(4, '0');
    request.append(lowByte);  // CRC 低字节
    request.append(highByte);    // CRC 高字节

    return request.toUpper();
}

QByteArray Serialport::buildWriteRequest(quint8 deviceAddr, quint16 registerAddr, quint16 value)
{
    QByteArray request;
    request.append(deviceAddr);
    request.append(0x06);
    request.append(static_cast<char>(registerAddr >> 8));  // 寄存器地址高字节
    request.append(static_cast<char>(registerAddr & 0xFF));  // 寄存器地址低字节

    request.append(static_cast<char>(value >> 8));  // 读取寄存器数据高字节
    request.append(static_cast<char>(value & 0xFF));  // 读取寄存器数据低字节


    quint16 crc = calculateCRC(request);
    uint8_t lowByte = crc & 0xFF;         // 低字节
    uint8_t highByte = (crc >> 8) & 0xFF; // 高字节
//    qDebug() << "CRC16 Low Byte:" << QString::number(lowByte, 16).toUpper().rightJustified(4, '0');
//    qDebug() << "CRC16 High Byte:" << QString::number(highByte, 16).toUpper().rightJustified(4, '0');
    request.append(lowByte);  // CRC 低字节
    request.append(highByte);    // CRC 高字节
    //qDebug()<<"buildWriteRequest:"<<request.toHex();

    return request;
}

void Serialport::readModbusData(quint8 deviceAddr, quint16 registerAddr, quint16 registerCount)
{
    QByteArray request = buildReadRequest(deviceAddr, registerAddr, registerCount);
    serialPort->write(request);
        QLOG_DEBUG() << "Sent read request:" << request.toHex();

        if (serialPort->waitForReadyRead(1000)) {  // 等待 1 秒
               QByteArray response = serialPort->readAll();
               QLOG_DEBUG() << "Received response:" << response.toHex();

               quint16 crcReceived = (static_cast<quint8>(response.at(response.size() - 1)) << 8)
                                   | static_cast<quint8>(response.at(response.size() - 2));

               QByteArray responseWithoutCRC = response.left(response.size() - 2);  // 去掉 CRC 部分
               quint16 crcCalculated = calculateCRC(responseWithoutCRC);

               if (crcReceived != crcCalculated) {
                   QLOG_DEBUG() << "CRC check failed!";
                   //return;
               }

               // 解析响应数据
               quint8 functionCode = static_cast<quint8>(response.at(1));
               if (functionCode == 0x03) {  // 确认是功能码 0x03
                   QString dataString;
                   quint8 byteCount = static_cast<quint8>(response.at(2));
                   for (int i = 0; i < byteCount / 2; ++i) {
                       quint16 registerValue = (static_cast<quint8>(response.at(3 + i * 2)) << 8)
                                             | static_cast<quint8>(response.at(4 + i * 2));
                       QLOG_DEBUG() << "Register" << registerAddr + i << "=" << registerValue;
                       dataString.append(QString("Register %1: %2\n").arg(registerAddr + i).arg(registerValue));
                   }
               } else {
                   QLOG_DEBUG() << "Unexpected function code:" << functionCode;
               }
           } else {
               QLOG_DEBUG() << "No response received!";
        }
}

void Serialport::writeModbusData(quint8 deviceAddr, quint16 registerAddr, quint16 value)
{
    QByteArray request = buildWriteRequest(deviceAddr, registerAddr, value);
    //qDebug()<<"writeModbusData:"<<request.toHex().toUpper();
    QLOG_DEBUG() << "send data:" << request.toHex().toUpper();
    serialPort->write(request);

    if (serialPort->waitForReadyRead(1000)) {  // 等待 1 秒
           QByteArray response = serialPort->readAll();
           QLOG_DEBUG() << "Received response:" << response.toHex();

           // 校验 CRC
//           if (response.size() < 5) {  // 最小响应长度为 5 字节
//               qDebug() << "Response too short!";
//               return;
//           }

//           quint16 crcReceived = (static_cast<quint8>(response.at(response.size() - 1)) << 8)
//                               | static_cast<quint8>(response.at(response.size() - 2));

//           QByteArray responseWithoutCRC = response.left(response.size() - 2);  // 去掉 CRC 部分
//           quint16 crcCalculated = calculateCRC(responseWithoutCRC);

//           if (crcReceived != crcCalculated) {
//               qDebug() << "CRC check failed!";
//               return;
//           }

           // 解析响应数据
           quint8 functionCode = static_cast<quint8>(response.at(1));
           if (functionCode == 0x06) {

               quint8 registerAddr = static_cast<quint8>(response.at(2));
               quint16 registerValue = (static_cast<quint8>(response.at(3)) << 8)
                                     | static_cast<quint8>(response.at(4));
               QLOG_DEBUG() << "Register" << registerAddr;
               QLOG_DEBUG() << "now value : " << registerValue;
           }

       } else {
           QLOG_DEBUG() << "write request No response received!";
    }
}

void Serialport::writeJOG()
{
    QByteArray request;
    request.append(0x3F);
    request.append(0x06);
    request.append(0x9e);
    request.append(0x09);
    request.append(0x2A);
    request.append(0x2A);
    request.append(0xEC);
    request.append(0x41);
    QLOG_DEBUG() << "JOG send data:" << request.toHex();

    serialPort->write(request);

    if (serialPort->waitForReadyRead(1000)) {  // 等待 1 秒
           QByteArray response = serialPort->readAll();
           QLOG_DEBUG() << "Received response:" << response.toHex();
    }else{
        QLOG_DEBUG() << "write request No response received!";

    }

}

void Serialport::writeJOG_()
{
    QByteArray request;
    request.append(0x3F);
    request.append(0x06);
    request.append(0x9e);
    request.append(0x09);
    request.append(0x4A);
    request.append(0x4A);
    request.append(0xC4);
    request.append(0x69);
    QLOG_DEBUG() << "JOG- send data:" << request.toHex();

    serialPort->write(request);

    if (serialPort->waitForReadyRead(1000)) {  // 等待 1 秒
           QByteArray response = serialPort->readAll();
           QLOG_DEBUG() << "Received response:" << response.toHex();
    }else{
        QLOG_DEBUG() << "write request No response received!";

    }
}

void Serialport::writeZero()
{
    QByteArray request;
    request.append(0x3F);
    request.append(0x06);
    request.append(0x9e);
    request.append(0x09);
    request.append(0x4A);
    request.append(0x4A);
    request.append(0xC4);
    request.append(0x69);
    QLOG_DEBUG() << "ZERO send data:" << request.toHex();

    serialPort->write(request);

    if (serialPort->waitForReadyRead(1000)) {  // 等待 1 秒
           QByteArray response = serialPort->readAll();
           QLOG_DEBUG() << "Received response:" << response.toHex();
    }else{
        QLOG_DEBUG() << "write request No response received!";

    }
}

QByteArray Serialport::buildPosRequest(quint8 deviceAddr, quint16 registerAddr, quint8 highVal, quint8 lowval)
{
    QByteArray request;
       request.append(deviceAddr);
       request.append(0x06);
       request.append(static_cast<char>(registerAddr >> 8));  // 寄存器地址高字节
       request.append(static_cast<char>(registerAddr & 0xFF));  // 寄存器地址低字节
       request.append(static_cast<char>(highVal));  // 读取寄存器个数高字节
       request.append(static_cast<char>(lowval));  // 读取寄存器个数低字节


       quint16 crc = calculateCRC(request);
       uint8_t lowByte = crc & 0xFF;         // 低字节
       uint8_t highByte = (crc >> 8) & 0xFF; // 高字节

       request.append(lowByte);  // CRC 低字节
       request.append(highByte);    // CRC 高字节
       //qDebug()<<"buildWriteRequest:"<<request.toHex();

       return request;

}

void Serialport::writeStepPosData(quint8 deviceAddr, quint16 registerAddr, quint8 value_high, qint8 value_low)
{
    QByteArray request = buildPosRequest(deviceAddr, registerAddr, value_high,value_low);
        //qDebug()<<"writeModbusData:"<<request.toHex().toUpper();
        QLOG_DEBUG() << "writeStepPosData send data:" << request.toHex().toUpper();
        serialPort->write(request);

        if (serialPort->waitForReadyRead(1000)) {  // 等待 1 秒
               QByteArray response = serialPort->readAll();
               QLOG_DEBUG() << "Received response:" << response.toHex();

        }


}

