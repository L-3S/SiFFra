#include <QtCore>

#include "FbsfNetGlobal.h"
#include "FbsfNetServer.h"
#include "FbsfNetLogger.h"
#include "FbsfTcpServer.h"
#include "FbsfSrvClient.h"
#include "FbsfNetProtocol.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfTcpServer
    \brief Listening tcp/ip network incomming clients
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfTcpServer::FbsfTcpServer(FbsfNetServer *aServer, QObject *parent)
    : QTcpServer(parent)
    , mServer(aServer)
    , clientList(QList<FbsfSrvClientThread*> ())
    , protectClientList()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfTcpServer::~FbsfTcpServer()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///  QTcpServer virtual method redefinition
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::incomingConnection(int socketDescriptor)
{
    FbsfSrvClientThread *srvClient = new FbsfSrvClientThread(socketDescriptor);
    QObject::connect(srvClient, SIGNAL(receivedMessage(QByteArray)), this, SLOT(processMsg(QByteArray)));
    QObject::connect(srvClient, SIGNAL(clientClosed(int)), this, SLOT(threadFinished(int)), Qt::DirectConnection);
    QObject::connect(srvClient, SIGNAL(finished()), srvClient, SLOT(deleteLater()));
    {
    QMutexLocker locker(&protectClientList);
    clientList.append(srvClient);// register new client
    }
    srvClient->start();
    // register new client to controler
    emit registerClient((void*)srvClient);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process incomming message from clients
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::processMsg(const QByteArray &msg)
{
    FbsfSrvClientThread *clSender = reinterpret_cast<FbsfSrvClientThread*>(sender());

    //FbsfLog_trace("NetServer", "Received message from client [" + clSender->getClient()->peerAddress().toString() + "]");

    FbsfNetProtocol protocol(msg);

    if (protocol.getHeader() == FbsfNetProtocol::CLIENT_INFO)
    {

        FbsfSrvClient *client = clSender->getClient();
        client->setName(protocol.Parameter().value("name").toString());

        //Notify the clients that new one is connected
        //and then the Server sends the client list to the new client
        FbsfNetProtocol newUser;
        newUser.setHeader(FbsfNetProtocol::NEW_CLIENT);
        newUser.setBody("New user connected");
        newUser.Parameter("user", protocol.Parameter().value("name").toString());

        FbsfNetProtocol clientsList;
        clientsList.setHeader(FbsfNetProtocol::CLIENTS_LIST);
        clientsList.setBody("List of connected clients");

        for (int i = 0; i < clientList.size(); i++)
        {
            if (clientList.at(i)->getClient()->getId() != client->getId())
            {
                clientList.at(i)->sendMessage(newUser.convertToStream());
                clientsList.Parameter("user"+QVariant(clientList.at(i)->getClient()->getId()).toString(),
                                      clientList.at(i)->getClient()->getName());
            }
        }
        clSender->sendMessage(clientsList.convertToStream());
        //client->waitForBytesWritten();
    }
    else if (protocol.getHeader() == FbsfNetProtocol::EXECUTIVE)
        executiveMsg(protocol);
    else if (protocol.getHeader() == FbsfNetProtocol::MULTICAST)
        multicastMsg(clSender->getClient(), protocol);
    else if (protocol.getHeader() == FbsfNetProtocol::MESSAGE)
        clientToClientMsg(protocol);
    else if (protocol.getHeader() == FbsfNetProtocol::DATA)
        publicData(protocol);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process incomming executive command (stop, run, freeze, step, ...)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::executiveMsg(const FbsfNetProtocol &p)
{
    // signal control message to SLOT Executive::control(QString)
    mServer->sendExecutiveMsg(p.getBody());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process incomming message to target client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::clientToClientMsg(const FbsfNetProtocol &p)
{
    QMutexLocker locker(&protectClientList);
    for (int i = 0; i < clientList.size(); i++)
    {
        if (clientList.at(i)->getClient()->getName() == p.Parameter("receiver") && p.getBody() != "")
        {
            clientList.at(i)->sendMessage(p.convertToStream());
            //clientList.at(i)->waitForBytesWritten();
            break;
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Broadcast incomming message to all node excluding source client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::multicastMsg(FbsfSrvClient *aSrvClient, const FbsfNetProtocol &p)
{
    if (aSrvClient != NULL)
    {
        FbsfNetProtocol newMsg;
        newMsg.setHeader(FbsfNetProtocol::MESSAGE);
        newMsg.setBody(p.getBody());
        newMsg.Parameter("sender", aSrvClient->getName());

        FbsfLog_trace("NetServer", "Server multicast message: " + p.getBody());
        QMutexLocker locker(&protectClientList);
        for (int i = 0; i < clientList.size(); i++)
        {
            // all nodes excluding sender client
            if (clientList.at(i)->getClient()->getId() != aSrvClient->getId())
                        clientList.at(i)->sendMessage(newMsg.convertToStream());
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process incomming public data (comming from remote client)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::publicData(const FbsfNetProtocol &p)
{
    emit sendPublicDataValue(p.getBody(),p.Parameter("value"));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Process disconnection of a remote client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::threadFinished(int threadId)
{
	QString userExited;
    {
    QMutexLocker locker(&protectClientList);
	for (int i = 0; i < clientList.size(); i++)
	{
		if (clientList.at(i)->getThreadId() == threadId)
		{
			userExited = clientList.at(i)->getClient()->getName();
			clientList.removeAt(i);
		}
	}
    }
	//Message to the other clients
    FbsfNetProtocol newUser;
    newUser.setHeader(FbsfNetProtocol::CLIENT_EXIT);
    newUser.setBody("User disconnected");
    newUser.Parameter("user", userExited);
	
	for (int i = 0; i < clientList.size(); i++)
        clientList.at(i)->sendMessage(newUser.convertToStream());

    FbsfLog_trace("NetServer", tr("User disconnected :") + userExited);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get clients list
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QList<FbsfSrvClient*> FbsfTcpServer::getClients()
{
    QList<FbsfSrvClient*> clients;
    QMutexLocker locker(&protectClientList);
    for (int i = 0; i < clientList.size(); i++)
        clients.append(clientList.at(i)->getClient());

    return clients;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get client by id
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSrvClient * FbsfTcpServer::getClient(int id)
{
    QMutexLocker locker(&protectClientList);
    for (int i = 0; i < clientList.size(); i++)
        if (clientList.at(i) && clientList.at(i)->getClient()->getId() == id)
            return clientList.at(i)->getClient();

    return NULL;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get client by name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSrvClient * FbsfTcpServer::getClientByName(const QString &_name)
{
    QMutexLocker locker(&protectClientList);
    for (int i = 0; i < clientList.size(); i++)
        if (clientList.at(i) && clientList.at(i)->getClient()->getName() == _name)
                return clientList.at(i)->getClient();

    return NULL;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Return client thread list for controler
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QList<FbsfSrvClientThread*> FbsfTcpServer::getSrvClients()
{
    QMutexLocker locker(&protectClientList);
    return clientList;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Send string message to all clients
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTcpServer::writeToClients(const QString &msg)
{
    QMutexLocker locker(&protectClientList);
	for (int i = 0; i < clientList.size(); i++)
    {
        FbsfNetProtocol clientMsg;
        clientMsg.setHeader(FbsfNetProtocol::MESSAGE);
        clientMsg.setBody(msg);
        clientList.at(i)->sendMessage(clientMsg.convertToStream());
    }
}
