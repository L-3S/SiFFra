#include <QtNetwork>

#include "FbsfSrvClient.h"
#include "FbsfNetServer.h"
#include "FbsfNetGlobal.h"
#include "FbsfNetLogger.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfNetServer
    \brief Network server
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetServer::FbsfNetServer(QObject *parent) : QObject(parent)
{
    tcpServer = new FbsfTcpServer(this);
    tcpServer->listen(QHostAddress::Any,SERVER_PORT);
	rootServerThread = new QThread();
	connect(rootServerThread, SIGNAL(finished()), rootServerThread, SLOT(deleteLater()));
    tcpServer->moveToThread(rootServerThread);
    rootServerThread->start();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetServer::~FbsfNetServer()
{
	rootServerThread->exit(0);
	rootServerThread->deleteLater();
    FbsfNetLoggerManager *manager = FbsfNetLoggerManager::getInstance();
	manager->closeLogger();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SIGNAL : push control message to server executive
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetServer::sendExecutiveMsg(QString aMsg)
{
    emit executiveMsg(aMsg);
}


