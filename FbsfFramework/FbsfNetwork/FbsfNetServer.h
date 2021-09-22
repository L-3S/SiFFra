#ifndef FbsfNetServer_H
#define FbsfNetServer_H

#include <QObject>
#include "FbsfTcpServer.h"
#include "FbsfNetGlobal.h"
#include <iostream>

using namespace std;

class FBSF_INTERFACE FbsfNetServer : public QObject
{
    Q_OBJECT

public:
        explicit FbsfNetServer(QObject *parent = 0);
        ~FbsfNetServer();
        void            sendExecutiveMsg(QString aMsg);
        FbsfTcpServer * getTcpServer() { return tcpServer;}
private:
        FbsfTcpServer * tcpServer;
        QThread *       rootServerThread;
signals:
        void            executiveMsg(QString);
};

#endif
