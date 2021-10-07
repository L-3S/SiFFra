#include "FbsfControler.h"
#include "FbsfPublicData.h"
#include "FbsfPublicDataModel.h"// sPublicDataModel
#include "FbsfNetServer.h"
#include "FbsfNetClient.h"

FbsfNetClient* FbsfControler::mNetClient=nullptr;
FbsfNetServer* FbsfControler::mNetServer=nullptr;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \class FbsfControler
/// \brief Network data controler
/// Management of incomming/outgoing public data between server and clients
/// This class works as an interface between FbsfPublicData and FbsfNetwork
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfControler::FbsfControler(QObject *parent) :
    QObject(parent)
{
#ifndef MODE_BATCH
    // QML Monitor update if Standalone or Client mode
    if  (mNetServer == nullptr)
    {
        connect(this,SIGNAL(updateMonitor()), FbsfdataModel::sFactoryListviewDataModel(),SLOT(updateValues()));
        connect(this,SIGNAL(updateMonitorList()), FbsfdataModel::sFactoryListviewDataModel(),SLOT(updateList()));
    }
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// CLIENT MODE
/// Register client network
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::setClientMode(FbsfNetClient* aClient)
{
    mNetClient = aClient;
    // Callback to push public data to executive
    FbsfDataExchange::executiveCB=&pushDataToServer;
    // SIGNAL for updating public data list from remote server
    connect(mNetClient,SIGNAL(sendPublicDataList(FbsfNetProtocol))
           ,this, SLOT(updatePublicDataList(FbsfNetProtocol)));
    // SIGNAL for updating public data values from remote server
    connect(mNetClient,SIGNAL(sendPublicDataValues(FbsfNetProtocol))
           ,this, SLOT(updatePublicDataValues(FbsfNetProtocol)));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SERVER MODE
/// Register server network
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::setServerMode(FbsfNetServer* aServer)
{
    mNetServer = aServer;
    // Slot for updating public data to remote client side
    connect(mNetServer->getTcpServer(),SIGNAL(registerClient(void*))
            ,this, SLOT(updateClientPublicDataList(void*)));
    // Slot for updating a single public data value comming from remote client
    connect(mNetServer->getTcpServer(),SIGNAL(sendPublicDataValue(QString,QVariant))
            ,this, SLOT(updateServerPublicDataValue(QString,QVariant)));

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SERVER MODE
/// Called just before Executive process loop
/// initialize data list to update new connected client
// !!!!! Called too early for QML subscribers on client side
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::initializePublicDataList()
{
    if (mNetServer)
    {
        msgDataList.setHeader(FbsfNetProtocol::DATASET);
        msgDataList.setBody("description");

        // Publish public data produced on the server side
        QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
        foreach (FbsfDataExchange* data, dataList)
        {
            // Register FbsfDataExchange for remote client
            if (data->FlagsAny(FbsfDataExchange::cExporter))
            {
                QStringList description;
                description.append(data->TypeToStr());
                description.append(data->producer());
                description.append(data->unit());
                description.append(data->description());
                // Scalar or Vector
                description.append(data->ClassToStr());
                description.append(QString::number(data->size()));

                msgDataList.Parameter(data->name(),description);
            }
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SERVER MODE
/// Called when new client connect : publish public data description to client
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::updateClientPublicDataList(void *aClient)
{
    protectPublish.lock();
    // new connected client
    FbsfSrvClientThread* client = static_cast<FbsfSrvClientThread*>(aClient);
    // Publish public data produced on the server side
    client->publishDataList(msgDataList);
    protectPublish.unlock();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SERVER MODE
/// Called from Executive process loop : transfer public data to clients
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::process(uint aStep)
{
    if (mNetServer)
    {
        protectPublish.lock();
        QMap<QString,QVariant> dataValues;
        QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
        foreach (FbsfDataExchange* data, dataList)
        {
            // get name/value from exported public data
            if (data->FlagsAny(FbsfDataExchange::cExporter))
            {
                if (data->Class()==cScalar)
                {
                    dataValues.insert(data->name(),data->value());
                }
                else// IMPLEMENT VECTOR
                {
                    QVariantList vectorValues;

                    for (int i=0;i < data->size();i++)
                        vectorValues.append(data->vectorValue(i));
                    dataValues.insert(data->name(),vectorValues);
                }
            }
        }
        // Push public data tag/value to clients
        const QList<FbsfSrvClientThread*>&
                clientList=mNetServer->getTcpServer()->getSrvClients();
        for (int iClient=0; iClient<clientList.size();iClient++)
        {
            // Publish public data produced on the server side
            if (clientList[iClient]->DataListPublished())
                clientList[iClient]->publishdataValues(dataValues);
        }
        protectPublish.unlock();
    }
    else if(mNetClient)// Client mode
    {
        // update is synchronized with reception of data values
        // see updatePublicDataValues()

        ///>>>>> TODO : see if processPublicData is needed

    }
    else // Standalone mode
    {
        FbsfDataExchange::processPublicData(aStep);// Restore,Record/Replay
#ifndef MODE_BATCH
        emit updateMonitor();// update only data values
#endif
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SERVER MODE
/// Push incomming single data to public data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::updateServerPublicDataValue(QString aTag,QVariant aValue)
{
    switch (aValue.type())
    {
    case QMetaType::Int:
            FbsfDataExchange::sPublicDataMap.value(aTag)->value(aValue.toInt());
            break;
    case QMetaType::Float :
            FbsfDataExchange::sPublicDataMap.value(aTag)->value(aValue.toFloat());
            break;
    default : break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// CLIENT MODE
/// Push data to server (called from FbsfPublicData)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::pushDataToServer(QString aTag,QVariant aValue)
{
    if(mNetClient) mNetClient->sendDataValue(aTag,aValue);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// CLIENT MODE
/// Update public data description
/// called from FbsNetClient when connection is established
/// Message : name ; [type (I/R) ; producer ; unit ; description; class; size]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::updatePublicDataList(FbsfNetProtocol aMsg)
{
    QMapIterator<QString, QVariant> i(aMsg.Parameter());
    while (i.hasNext())
    {
        i.next();
        QStringList dataDescription = i.value().toStringList();
        // declare public data exported from server side
        FbsfDataExchange::declarePublicData(i.key(),
                                            FbsfDataExchange::StrToType(dataDescription.at(0)),
                                            FbsfDataExchange::cExporter,
                                            dataDescription.at(1),
                                            dataDescription.at(2),
                                            dataDescription.at(3),
                                            FbsfDataExchange::StrToClass(dataDescription.at(4)),
                                            dataDescription.at(5).toInt()
                                            );
    }
#ifndef MODE_BATCH
    emit updateMonitorList();
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// CLIENT MODE
/// Update public data values (called from FbsNetClient) via signal sendPublicDataValues
/// Message : [ name ; value ]*
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::updatePublicDataValues(FbsfNetProtocol aMsg)
{
    QMapIterator<QString, QVariant> i(aMsg.Parameter());
    while(i.hasNext()){i.next();updateClientPublicDataValue(i.key(),i.value());}
#ifndef MODE_BATCH
    emit updateMonitor();// update only data values
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// CLIENT MODE
/// Push incomming single data to public data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfControler::updateClientPublicDataValue(QString aTag,QVariant aValue)
{// IMPLEMENT VECTOR
    if (!FbsfDataExchange::sPublicDataMap.contains(aTag)) return;

    switch (aValue.type())
    {
    case QMetaType::Int:
            FbsfDataExchange::sPublicDataMap.value(aTag)->setIntValue(aValue.toInt());
            break;
    case QMetaType::Float :
            FbsfDataExchange::sPublicDataMap.value(aTag)->setRealValue(aValue.toFloat());
            break;
    case QMetaType::QVariantList : // Vector
            FbsfDataExchange::sPublicDataMap.value(aTag)->setData(aValue.toList());
            break;

    default : break;
    }
}



