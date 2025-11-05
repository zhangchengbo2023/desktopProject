#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QMap>
class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr)
        : QTcpServer(parent){
        nextClientId = 1;
    }
    void startServer(quint16 port){
        if(!this->listen(QHostAddress::Any,port)){
            qDebug()<<"start tcp server failed.";
        }else{
            qDebug()<<"start tcp server at port:" <<port;;
        }
    }

signals:
    void messageReceived(QString message, int clientId);
protected:
    void incomingConnection(qintptr handle) override;
public slots:
    void sendMessageToClient(const QString& message,int clientId){
        qDebug()<<"sending msg:"<<message<< "to client :"<<QString::number(clientId);
        if(clients.contains(clientId)){
            clients[clientId]->write(message.toUtf8());
        }
    }
private:
    QHash <int,QTcpSocket*> clients;
    int nextClientId;
};

#endif // TCPSERVER_H
