#include "FbsfSrvClient.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfSrvClient
    \brief Client manager on server side
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSrvClient::FbsfSrvClient(const int id,const QString &name,
                             QObject *parent)
    : QTcpSocket(parent)
{
    if (id != -1)        setSocketDescriptor(id);
    if (name != "")      m_name = name;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSrvClient & FbsfSrvClient::operator = (const FbsfSrvClient &c)
{
    if (this != &c)
    {
        m_name = c.m_name;
        setSocketDescriptor(c.getId());
    }
    return *this;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Threaded network client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "FbsfNetGlobal.h"
#include "FbsfNetLogger.h"
#include "FbsfNetProtocol.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfSrvClientThread
    \brief Threaded Client manager on server side
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSrvClientThread::FbsfSrvClientThread(int id, QObject *parent)
    : QThread(parent)
    , srvClient(0)
    , socketId (id)
    , blockSize (0)
    , mDataListPublished(false)
{
    quit = false;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Thread procedure
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSrvClientThread::run()
{
    srvClient = new FbsfSrvClient(socketId);

    QObject::connect(srvClient, SIGNAL(readyRead())
                     ,this, SLOT(readyRead()), Qt::DirectConnection);
    QObject::connect(srvClient, SIGNAL(disconnected())
                     ,this, SLOT(disconnected()), Qt::DirectConnection);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // send acknowledgment
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FbsfNetProtocol clientMsg;
    clientMsg.setHeader(FbsfNetProtocol::MESSAGE);
    clientMsg.setBody("Connected to the Server");
    sendMessage(clientMsg.convertToStream());
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FbsfLog_trace("NetSrvClient", "New client from " + srvClient->peerAddress().toString());
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    while (!quit)
    {
        srvClient->waitForReadyRead(1);
        // process all pending messages
        for (int i = msgToWrite.size() - 1; i >= 0; i--)
        {
            srvClient->write(msgToWrite.at(i));
            msgToWrite.removeAt(i);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Slot : process socket incomming data
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSrvClientThread::readyRead()
{
    QDataStream in(srvClient);
    in.setVersion(QDataStream::Qt_5_0);

    if (blockSize == 0)
    {
        if (srvClient->bytesAvailable() < (int)sizeof(quint32)) return;
        in >> blockSize;
    }
    if (srvClient->bytesAvailable() < blockSize) return;

    emit receivedMessage(srvClient->readAll());
    blockSize=0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Disconnection of remote client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSrvClientThread::disconnected()
{
    emit clientClosed(socketId);
    srvClient->deleteLater();
    quit = true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Send message to network client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSrvClientThread::sendMessage(const QByteArray &msg)
{
    //QLog_Trace("ClientThread", "FbsfSrvClientThread::sendMessage()");
    msgToWrite.prepend(msg);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Publish list of public data to remote client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSrvClientThread::publishDataList(FbsfNetProtocol& aMsg)
{
    mDataListPublished=true;
    sendMessage(aMsg.convertToStream());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Publish list of public data values to remote client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSrvClientThread::publishdataValues(QMap<QString,QVariant>& aValues)
{
    FbsfNetProtocol dataValuesMsg;
    dataValuesMsg.setHeader(FbsfNetProtocol::DATASET_VALUES);
    dataValuesMsg.setBody("values");
    foreach (QString tag,aValues.keys())
    {
        dataValuesMsg.Parameter(tag,aValues.value(tag));
    }
    // send message
    sendMessage(dataValuesMsg.convertToStream());

}
