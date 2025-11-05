#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

class Serialport : public QObject
{
    Q_OBJECT
public:
    explicit Serialport(QObject *parent = nullptr);
    void setPortName(QString portName);
    bool openSerialPort();
    void closeSerialPort();

    void readModbusData(quint8 deviceAddr,quint16 registerAddr,quint16 registerCount);
    void writeModbusData(quint8 deviceAddr,quint16 registerAddr,quint16 value);
    void writeJOG();
    void writeJOG_();
    void writeZero();
    void writeModbusDataResult(quint16 xpos,quint16 ypos,quint16 zpos,quint16 fifth,quint16 cangel,quint16 result);
    QByteArray buildPosRequest(quint8 deviceAddr, quint16 registerAddr, quint8 highVal, quint8 lowval);
    void writeStepPosData(quint8 deviceAddr, quint16 registerAddr, quint8 value_high, qint8 value_low);

signals:
    void dataReceived(QString& data);
private:
    QSerialPort* serialPort;
    QVector<uint8_t> convertToHex(int32_t number,size_t byteSize){
            QVector<uint8_t> result(byteSize,0);
            uint32_t unsignedNumber = static_cast<uint32_t>(number);
            if(number < 0){
                unsignedNumber = static_cast<uint32_t>(number + (1LL << (byteSize * 8)));
            }

            for(size_t i = 0; i < byteSize; ++i){
                result[byteSize - 1 - i] = static_cast<uint8_t>((unsignedNumber >> (8 * i)) & 0xFF);
            }
            return result;
        }
    unsigned short  calculateCRC(const QByteArray &data);
    QByteArray buildReadRequest(quint8 deviceAddr,quint16 registerAddr,quint16 registerCount);
    QByteArray buildWriteRequest(quint8 deviceAddr,quint16 registerAddr,quint16 value);
};

#endif // SERIALPORT_H
