#ifndef Fbsf_TcpServer_H
#define Fbsf_TcpServer_H
 
#include <QStringList>
#include <QTcpServer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include "FbsfNetGlobal.h"
#include "FbsfSrvClient.h"
#include "FbsfNetProtocol.h"
class FbsfNetServer;
class FBSF_INTERFACE FbsfTcpServer : public QTcpServer
{
    Q_OBJECT

    public:
        FbsfTcpServer(FbsfNetServer* aServer,QObject *parent = 0);
        ~FbsfTcpServer();

        FbsfSrvClient *             getClient(int id);
        FbsfSrvClient *             getClientByName(const QString &_name) ;
        QList<FbsfSrvClient*>       getClients() ;
        QList<FbsfSrvClientThread*> getSrvClients();

        void                        writeToClients(const QString &msg);

    protected:
        void                        incomingConnection(int socketDescriptor);

    signals:
        // from client to server
        void                        sendPublicDataValue(QString,QVariant);
        void                        registerClient(void*);

    private:        
        void clientToClientMsg(const FbsfNetProtocol &p);
        void executiveMsg(const FbsfNetProtocol &p);
        void publicData(const FbsfNetProtocol &p);
        // from server to client
        void publicDataSet(const FbsfNetProtocol &p);

    private slots:
        void processMsg(const QByteArray &msg);
        void threadFinished(int threadId);
        void multicastMsg(FbsfSrvClient *client, const FbsfNetProtocol &p);
    private :
        FbsfNetServer*                 mServer;
        QList<FbsfSrvClientThread*>    clientList;
        QMutex                         protectClientList;

};

#endif
