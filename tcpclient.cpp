#include "tcpclient.h"


#include <QHostAddress>
#include <QDebug>
#include "QsLog/QsLog.h"
TcpClient::TcpClient(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this))
{
    // 连接信号槽
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
}

void TcpClient::connectToServer(const QString &host, quint16 port)
{
    // 发起连接
    socket->connectToHost(host, port);
}

void TcpClient::sendData(const QString &data)
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        // 将数据写入套接字并发送
        socket->write(data.toUtf8());
        QLOG_DEBUG() << "send data to server:"<<data;
    } else {
        QLOG_DEBUG() << "Not connected to tcp server";
    }
}

void TcpClient::onConnected()
{
    QLOG_DEBUG() << "Connected to tcp server";
}

void TcpClient::onReadyRead()
{
    // 读取所有可用数据
    QByteArray data = socket->readAll();
    QString receivedData = QString::fromUtf8(data);
    QLOG_DEBUG() << "收到数据:"<<receivedData;
    // 发出信号，传递收到的数据
    emit dataReceived(receivedData);
}

void TcpClient::onDisconnected()
{
    QLOG_DEBUG() << "Disconnected from server";
}
