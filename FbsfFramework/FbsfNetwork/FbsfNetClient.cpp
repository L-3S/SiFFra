#include "FbsfNetClient.h"
#include "FbsfNetLogger.h"
#include <QtNetwork>

int     FbsfNetClient::PORT = SERVER_PORT;
QString FbsfNetClient::user = "unknown";

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfNetClient
    \brief Client network utilities for connection with server
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetClient::FbsfNetClient(const QString &aClientName,
                             QObject *parent)
    : QObject(parent)
    , mutex(QMutex::Recursive)
    , blockSize(0)
{
    user = aClientName;

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readMsg()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// query state of connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::connectServer(const QString &aHostName)
{
    socket->connectToHost(aHostName, PORT);
    socket->waitForConnected(10000);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// query state of connection
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfNetClient::status()
{
    return (socket->state() == QAbstractSocket::ConnectedState);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// destructor : close connection and delete
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetClient::~FbsfNetClient()
{
    socket->close();
    socket->deleteLater();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Send identification message
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::connected()
{
    QMutexLocker locker(&mutex);

    FbsfNetProtocol p;
    p.setHeader(FbsfNetProtocol::CLIENT_INFO);
    p.setBody("Client name");
    p.Parameter("name",user);

    socket->write(p.convertToStream());
    socket->waitForBytesWritten();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Send message to ALL Executive instances
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::sendExecutiveMsg(QString aBody)
{
    QMutexLocker locker(&mutex);

    FbsfNetProtocol p;

    p.setHeader(FbsfNetProtocol::EXECUTIVE);
    p.setBody(aBody);
    p.Parameter("sender",user);
    socket->write(p.convertToStream());
    socket->waitForBytesWritten();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Send message to ALL clients
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::broadcastMsg(QString aBody)
{
    QMutexLocker locker(&mutex);

    FbsfNetProtocol p;

    p.setHeader(FbsfNetProtocol::MULTICAST);
    p.setBody(aBody);
    p.Parameter("sender",user);
    socket->write(p.convertToStream());
    socket->waitForBytesWritten();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Send message to a single client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::singleClientMsg(QString aClientTarget, QString aBody)
{
    QMutexLocker locker(&mutex);

    FbsfNetProtocol p;

    p.setHeader(FbsfNetProtocol::MESSAGE);
    p.setBody(aBody);
    p.Parameter("receiver",aClientTarget);
    p.Parameter("sender",user);

    socket->write(p.convertToStream());
    socket->waitForBytesWritten();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Send data value to server (called from client executive)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::sendDataValue(QString aDataName, QVariant aValue)
{
    QMutexLocker locker(&mutex);

    FbsfNetProtocol p;
    p.setHeader(FbsfNetProtocol::DATA);
    p.setBody(aDataName);
    p.Parameter("value",aValue);
    socket->write(p.convertToStream());
    socket->waitForBytesWritten();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process incoming messages
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::readMsg()
{
    QMutexLocker locker(&mutex);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_0);
    if (blockSize == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint32)) return;
        in >> blockSize;
    }
    if (socket->bytesAvailable() < blockSize) return;
    FbsfNetProtocol p(socket->read(blockSize));
    blockSize=0;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    switch (p.getHeader())
    {
    case FbsfNetProtocol::NEW_CLIENT:
    {
        FbsfLog_trace ("NetClient",p.Parameter("user").toString() + tr(" connected."));
        break;
    }
    case FbsfNetProtocol::CLIENT_EXIT:
    {
        FbsfLog_trace ("NetClient",p.Parameter("user").toString() + tr(" disconnected."));
        break;
    }
    case FbsfNetProtocol::CLIENTS_LIST:
    {
        QList<QVariant> clientsName = p.Parameter().values();

        for (int i = 0; i < clientsName.size(); i++)
            FbsfLog_trace ("NetClient",tr("Clients : ")+clientsName.at(i).toString());
        break;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case FbsfNetProtocol::MESSAGE:
    {
        FbsfLog_trace ("NetClient",p.Parameter("sender").toString() + tr("reply : ") + p.getBody());
        break;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Public data communication btw Server and Client
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case FbsfNetProtocol::DATASET: publicDataSet(p);
        break;
    case FbsfNetProtocol::DATASET_VALUES: publicDataSetValues(p);
        break;
    default : break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process incomming public dataset (comming from remote server)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::publicDataSet(const FbsfNetProtocol &aMsg)
{
    emit sendPublicDataList(aMsg);//notify controler
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process incomming public dataset (comming from remote server)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::publicDataSetValues(const FbsfNetProtocol &aMsg)
{
    emit sendPublicDataValues(aMsg);//notify controler
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Client network utilities for connection with server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::restoreConnection()
{
    FbsfLog_trace ("NetClient",tr("Connecting to server..."));
    socket->connectToHost(QHostAddress::LocalHost, PORT);

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Network socket error notification
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetClient::displayError(QAbstractSocket::SocketError socketError)
{
     switch (socketError)
     {
     case QAbstractSocket::RemoteHostClosedError:
         FbsfLog_trace("NetClient", tr("FBSF Network Client : Remote host closed"));
         break;
     case QAbstractSocket::HostNotFoundError:
         FbsfLog_trace("NetClient", tr("FBSF Network Client : The host was not found"));
         break;
     case QAbstractSocket::ConnectionRefusedError:
         FbsfLog_trace("NetClient", tr("The connection was refused by the peer."));
         break;
     default:
         FbsfLog_trace("NetClient", tr("The following error occurred: %1.")
                                .arg(socket->errorString()));
     }
}
