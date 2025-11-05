#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QObject>
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QQueue>
#include <QTimer>
#include <QDebug>
#include "QsLog/QsLog.h"
struct DataUnit {
    float x;
    float y;
    float z;
    float r1;

    float c;
    int flag;  // 0: 合格, 1: 不合格
    float fth;
};
class Modbusclient : public QObject
{
    Q_OBJECT
public:
    explicit Modbusclient(QObject *parent = nullptr);
    ~Modbusclient(){
        if(modBusDevice != nullptr){
            disconnectFromServer();
            if(modBusDevice)
                modBusDevice->deleteLater();
        }

    }
    void connectToServer(const QString &addr,int port);
    void disconnectFromServer(){
        if(modBusDevice ->state() == QModbusDevice::ConnectedState){
            modBusDevice->disconnectDevice();
        }
    }
    void setUpdateStatus(bool state){
        m_updateState = state;
        QLOG_DEBUG()<<"update status:"<<m_updateState;
    }
    bool getUpdateStatus(){
        return m_updateState;
    }
    float registersToFloat(uint16_t highRegister, uint16_t lowRegister);
    void float_to_registers(float f, uint16_t* registers);
    void floatToRegisters(float value, uint16_t &highRegister, uint16_t &lowRegister);
    void readRegisters(int startAddr,int numberOfRegisters);
    void writeRegister(int address, int value);
    void writeFloatRegister(int address,float value);
    void writeIntRegister(int address,uint16_t value);
    void writeMultiPleFloatRegister(int address,QVector<float>& values);//一次性写入多个float
    void writeMultiPleIntRegister(int address,QVector<int>& values);//一次性写入多个int
    void writeMultiPleSingleIntRegister(int address,QVector<int>& values);//一次性写入多个float
    void writeMultipleRegistersWithGap(const QVector<quint16> &addresses, const QVector<quint16> &values);

    //数据存储
    void addDataUnit(float x, float y,float z,float r1,float c, int flag,float fth = 0) {
            DataUnit unit = {x, y,z,r1,c,flag,fth};
            queue.enqueue(unit);
            QLOG_DEBUG() << "Added DataUnit:" << "X:" << x << ",Y:" << y <<",Z:"<<z<<"r1:"<<r1<<",c:"<<c<< "Flag:" << flag<<"fth:"<<fth;
            QLOG_DEBUG()<<"now queue size :"<<queue.size();
        }
    void clearData(){
        QLOG_DEBUG()<<"queue cleared.now size :"<<queue.size();
        queue.clear();
    }
    bool QueueNotEmpty(){
        if (queue.isEmpty()){
            return false;
        }else{
            return true;
        }
    }

public slots:
    void onStateChanged(int state);
    void onWriteFinished(QModbusReply* reply);
    void onErrorOccurred(QModbusDevice::Error error);

    //数据输出
    void processQueue() {
            if (!queue.isEmpty()) {
                DataUnit unit = queue.dequeue();
                QLOG_DEBUG() << "Sending DataUnit to Modbus:" << "X:" << unit.x << "Y:" << unit.y <<"z:"<<unit.z<<"r1:"<<unit.r1<<"c:"<<unit.c<< "Flag:" << unit.flag;
                QLOG_DEBUG()<<"now queue size :"<<queue.size();
                // 写入 X, Y, Flag 到 Modbus 寄存器   //c 253.42  74.24

                //3个float一次写入寄存器
                QVector<float> float_vec(14);
                float_vec.clear();
                float_vec.push_back(unit.x);
                float_vec.push_back(0);
                float_vec.push_back(unit.y);
                float_vec.push_back(0);
                float_vec.push_back(unit.z);//Z轴 -125.0
                float_vec.push_back(0);
                float_vec.push_back(unit.r1);
                float_vec.push_back(0);
                float_vec.push_back(unit.fth*1000);//第五轴角度  *1000   3108
                float_vec.push_back(0);
                float_vec.push_back(unit.c);//C角度 3110
                float_vec.push_back(0);
                float_vec.push_back(unit.flag);//flag 3112
                //float_vec.push_back(0);

                //一次float坐标写入
                writeMultiPleFloatRegister(3100,float_vec);

                //全部写入整数
//                QVector<int> int_vec(14);
//                int_vec.clear();
//                int_vec.push_back(int(unit.x));
//                int_vec.push_back(0);

//                int_vec.push_back(int(unit.y));
//                int_vec.push_back(0);

//                int_vec.push_back(int(unit.z));//Z轴 -125.0
//                int_vec.push_back(0);

//                int_vec.push_back(int(unit.r1));
//                int_vec.push_back(0);

//                int_vec.push_back(int(unit.fth*1000));//第五轴角度  *1000   3108
//                int_vec.push_back(0);

//                int_vec.push_back(int(unit.c));//C角度 3110
//                int_vec.push_back(0);

//                int_vec.push_back(int(unit.flag));//flag 3112
//                writeMultiPleSingleIntRegister(3100,int_vec);


                //writeRegister(4522, 1);//第五轴运动完成



            } else {
                QLOG_DEBUG() << "DataManager Queue is empty.";
            }
        }
    void remains(){


        //3个float一次写入寄存器
        QVector<float> float_vec(8);
        float_vec.clear();
        float_vec.push_back(0);
        float_vec.push_back(0);
        float_vec.push_back(0);
        float_vec.push_back(0);
        float_vec.push_back(0);//Z轴 -125.0
        float_vec.push_back(0);
        float_vec.push_back(0);
        float_vec.push_back(0);
        //一次float坐标写入
        writeMultiPleFloatRegister(3100,float_vec);
        //剩下整数单独写入

        QVector<int> int_vec(6);
        int_vec.clear();

        int_vec.push_back(0);//第五轴角度  *1000   3108
        int_vec.push_back(0);
        int_vec.push_back(0);//C角度 3110
        int_vec.push_back(0);
        int_vec.push_back(6);//      3112
        int_vec.push_back(0);
        writeMultiPleIntRegister(3108,int_vec);


    }
signals:
    void readValue(int addr,int value);
    void setZero();
private:
    bool m_updateState = true;
    QModbusTcpClient* modBusDevice;
    QQueue<DataUnit> queue;
    QTimer* timer;
};

#endif // MODBUSCLIENT_H
