#ifndef FBSFCONTROLER_H
#define FBSFCONTROLER_H

#include <QObject>
#include <QVariant>
#include <QMutex>

#include "FbsfGlobal.h"
#include "FbsfNetProtocol.h"
class FbsfNetClient;
class FbsfNetServer;
class FbsfNetProtocol;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


class FBSF_FRAMEWORKSHARED_EXPORT FbsfControler
        : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FbsfControler)
public:
    explicit        FbsfControler(QObject *parent = nullptr);
    void            setClientMode(FbsfNetClient*);  // Client mode
    void            setServerMode(FbsfNetServer*);  // Server mode
    void            initializePublicDataList();     // resolve data link
    void            process(uint aStep);             // Manage data link in all case

    static void     pushDataToServer(QString aTag,QVariant aValue); // Client data to server

signals:
    void            updateMonitor();                // update QML monitor values
    void            updateMonitorList();            // update QML monitor list
public slots:
    // client side
    void            updatePublicDataList(FbsfNetProtocol aMsg); // description of public data
    void            updatePublicDataValues(FbsfNetProtocol aMsg); // values of public data
    void            updateClientPublicDataValue(QString aTag,QVariant aValue); // update single public data
    // server side
    void            updateClientPublicDataList(void* aClient); // for new registred client
    void            updateServerPublicDataValue(QString aTag, QVariant aValue); // update single public data
// members
private :
    static FbsfNetClient*   mNetClient;             // Network Client
    static FbsfNetServer*   mNetServer;             // Network Server
    FbsfNetProtocol         msgDataList;            // Public dataset description

    QMutex                  protectPublish;

};
#endif // FBSFCONTROLER_H
