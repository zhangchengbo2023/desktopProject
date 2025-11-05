#include "tcpserver.h"


void TcpServer::incomingConnection(qintptr handle)
{
    QTcpSocket* clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(handle);

    int clientId = nextClientId++;
    clients[clientId] = clientSocket;//save client socket

    connect(clientSocket,&QTcpSocket::readyRead,this,[this,clientId](){
        QTcpSocket* socket = clients[clientId];
        QByteArray data = socket->readAll();
        emit messageReceived(QString::fromUtf8(data),clientId);
    });

    connect(clientSocket,&QTcpSocket::disconnected,this,[this,clientId](){
        clients[clientId]->deleteLater();
        clients.remove(clientId);
        qDebug()<<"client "<<clientId <<"disconnected";
    });

    qDebug()<<"client connected.clientId :"<<clientId<<",ip:"<<clientSocket->peerAddress();
}
