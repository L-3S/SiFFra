#ifndef FbsfSrvClient_H
#define FbsfSrvClient_H

#include <QTcpSocket>
#include <QHostAddress>

#include <QThread>
#include <QMutex>
#include <QHash>
#include <QWaitCondition>

#include "FbsfNetGlobal.h"
#include "FbsfNetProtocol.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// network tcp socket client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfSrvClient : public QTcpSocket
{
    Q_OBJECT

    public:
        FbsfSrvClient(  int id = -1, const QString &name = "", QObject *parent = 0);
        FbsfSrvClient & operator = (const FbsfSrvClient &c);

        void                setName(const QString &_name) { m_name = _name; }
        QString             getName() const { return m_name; }
        void                setId(int id) { setSocketDescriptor(id); }
        int                 getId() const { return socketDescriptor(); }
        QString             getIpAddress() const { return peerAddress().toString(); }

    private:
        QString             m_name;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Threaded network client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class FBSF_INTERFACE FbsfSrvClientThread
        : public QThread
{
    Q_OBJECT

public:
    explicit            FbsfSrvClientThread(int id, QObject *parent = 0);
    void                run();
    void                sendMessage(const QByteArray &msg);
    FbsfSrvClient *     getClient() const { return srvClient; }
    int                 getThreadId() const { return socketId; }
    // Interface for FbsfControler
    void                publishDataList(FbsfNetProtocol &aMsg);        // publish data to client
    bool                DataListPublished(){return mDataListPublished;}
    void                publishdataValues(QMap<QString,QVariant>& aValues);// push data values to subscriber

signals:
    void                receivedMessage(const QByteArray &msg);
    void                clientClosed(int id);

public slots:
    void                readyRead();
    void                disconnected();
    // members
private:
    FbsfSrvClient *             srvClient;
    bool                        quit;
    QList<QByteArray>           msgToWrite;
    int                         socketId;
    quint32                     blockSize;

    bool                        mDataListPublished;

};


#endif // FbsfSrvClient_H
