#ifndef FBSFNETCLIENT_H
#define FBSFNETCLIENT_H

#include <QTcpSocket>
#include <QMutex>
#include "FbsfNetGlobal.h"
#include "FbsfNetProtocol.h"

class FBSF_INTERFACE FbsfNetClient : public QObject
{
    Q_OBJECT

public:
        FbsfNetClient(const QString &aClientName = "",
                      QObject *     parent = 0);
        ~FbsfNetClient();
        void            connectServer(const QString &aHostName);
        // connection state
        bool            status();
        // outgoing messages
        void            sendExecutiveMsg(QString aBody);
        void            broadcastMsg(QString aBody);
        void            singleClientMsg(QString aClientTarget,QString aBody);
        void            sendDataValue(QString aDataName, QVariant aValue);
        // incomming messages
        void            publicDataSet(const FbsfNetProtocol &aMsg);
        void            publicDataSetValues(const FbsfNetProtocol &aMsg);

public slots:
        void            setUserLogged(const QString &aClientName) { user = aClientName; }

signals:
    void                updateModel();
    void                sendPublicDataList( FbsfNetProtocol aMsg);
    void                sendPublicDataValues( FbsfNetProtocol aMsg);

private slots:
        void            connected();
        void            readMsg();
        void            restoreConnection();
        void            displayError(QAbstractSocket::SocketError socketError);

private:
        QMutex          mutex;
        static QString  IP;
        static int      PORT;
        static QString  user;
        QTcpSocket *    socket;
        quint32         blockSize;

};

#endif // FBSFNETCLIENT_H
