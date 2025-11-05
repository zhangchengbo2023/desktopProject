#ifndef TCPCLIENT_H
#define TCPCLIENT_H


#include <QTcpSocket>
#include <QObject>

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);
    void sendData(const QString &data);

signals:
    void dataReceived(const QString &data);

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket *socket;
};
#endif // TCPCLIENT_H
