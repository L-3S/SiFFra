#include "FbsfBaseModel.h"
#include "FbsfPublicData.h"
#include "FmuWrapper.h"

#include <QVector>
#include <QFileInfo>
#include <QUrl>
#include <QThread>
#include <QLibrary>
#include <QElapsedTimer>
//#ifndef MODE_BATCH
//#include <QtWidgets/QMessageBox>
//#endif


//#define TRACE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FBSFBaseModel
    \brief Base class of models under control of an Executive System
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FBSFBaseModel::FBSFBaseModel()
    : mQmlPlugin(nullptr)
    , mName()
    , mStatus (0)
    , mRootWindow(nullptr)
    , mSimulationTime(0)
    , mFirstStep(true)// set to false after prestep
{
    FBSFBaseModel::StateData::registerTypes();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FBSFBaseModel::~FBSFBaseModel()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::loadModule
///        loader for standard module
/// \param aModuleType : module class
/// \param aModuleName : module name
/// \param aRootWindow : graphic root window
/// \return
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FBSFBaseModel* FBSFBaseModel::loadModule(QString aModuleType,
                                         QString aModuleName,
                                         float   aTimeStep,
                                         QObject* aRootWindow)
{
    qInfo() << "\tModule : "
            << "Type"          << aModuleType
            << "Instance : "   << aModuleName;

    QLibrary sharedLibrary(aModuleType);

    typedef  FBSFBaseModel* (*ModelFactory)();
    ModelFactory factoryFunction = reinterpret_cast<ModelFactory>( sharedLibrary.resolve("factory"));
    if (factoryFunction)
    {
        FBSFBaseModel* pModel = factoryFunction();
        pModel->name(aModuleName);
        pModel->timeStep(aTimeStep);
        pModel->rootWindow(aRootWindow);

        return pModel;
    }
    else
    {
        DEBUG    <<  "Error while loading module" << aModuleType;
        qDebug() << sharedLibrary.errorString();
        qDebug() << "1. Check if the dll is in the library PATH";
        qDebug() << "2. Check if the dll dependencies are in the library PATH";
        return nullptr;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::loadFMUModel
///        loader for FMU model
/// \param aModelName : model name
/// \param aStep : simulation time step
/// \param aStart : start time
/// \param aEnd : end time
/// \param aDumpCsv : dump to csv flag
/// \return : pointer to base class
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FBSFBaseModel* FBSFBaseModel::loadFMUModel(QString aModulelName, QString aModelName,
                                           float aStep, float aStart, float aEnd,
                                           bool aDumpCsv)
{
    qInfo() << "\tModel : " << "Type FMU "
            << "Instance : " << aModelName
            << "timestep : " << aStep;

    FmuWrapper* FMUModel = new FmuWrapper(aStep,aStart,aEnd,aDumpCsv);
    FMUModel->name(aModulelName);
    if (!FMUModel->loadFMU(aModelName))
    {
        return nullptr;
    }
    else
    {
#ifdef TRACE
        qDebug() << "FBSFBaseModel : "  << aModelName << "FMU model Loaded ";
        qDebug() << "\t\ttimestep : "  << aStep ;
#endif
        return dynamic_cast<FBSFBaseModel*>(FMUModel);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::initialize
///        Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::initialize()
{
    #ifdef TRACE
    qDebug() << "FBSFBaseModel : " << name() <<"initialized in thread: "
             << QThread::currentThreadId();
    #endif
    mStatus = doInit();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::finalize
///        finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::finalize()
{
    #ifdef TRACE
    qDebug()<< "FBSFBaseModel : " << name() <<"terminated in thread: "
            << QThread::currentThreadId();
    #endif
    mStatus = doTerminate();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::consumeData
///        Data consumption procedure : signal to virtual method call
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::consumeData()
{
    preStep();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::computeStep
///        Step computation procedure : signal virtual method call
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::computeStep(int timeOut)
{
    QElapsedTimer timer;
    timer.start();
    mStatus = doStep(timeOut);
    if(mCpuStepTime!=nullptr)
        mCpuStepTime->setIntValue(timer.elapsed());
    postStep();

    mSimulationTime+=mTimeStep;// seconds
}
int FBSFBaseModel::doStep(int timeOut)
{
    return doStep();
}
int FBSFBaseModel::doStep()
{
    //XXX temporary fix waiting for consistency with timeout
    qDebug() << "Abstract method should not be called";
    return -1;
}
void FBSFBaseModel::cancelStep()
{
    mStepRunning = false;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::preStep
///        Get input values from public data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::preStep()
{
    QList<QPair<void*,FbsfDataExchange*>>::iterator iter;
    // foreach subscribed data, get input values from public data pool
    for (iter = mConsumedData.begin(); iter != mConsumedData.end(); ++iter)
    {
        FbsfDataExchange* iDataRef = (*iter).second;// public data

        if (iDataRef->Class() == cScalar)
        {   // Scalar
            if (iDataRef->Type()==cInteger)
                *(int*)((*iter).first)=iDataRef->getIntValue();
            else
                *(float*)((*iter).first)=iDataRef->getRealValue();
        }
        else // Vector
        {
            if (iDataRef->Type()==cInteger)
            {
                QVector<int>* pVector = (QVector<int>*)(*iter).first;
                // TODO VECTOR : check if adjust size is relevant (late declaration)
                if (mFirstStep) pVector->resize(iDataRef->size());
                memcpy(pVector->data(),iDataRef->p_data,iDataRef->size()*sizeof(int));
            }
            else
            {
                QVector<real>* pVector = (QVector<real>*)(*iter).first;
                if (mFirstStep) pVector->resize(iDataRef->size());
                memcpy(pVector->data(),iDataRef->p_data,iDataRef->size()*sizeof(real));
            }
        }
    }
    mFirstStep=false;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::postStep
///        push output values to public data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::postStep()
{
    QList<QPair<void*,FbsfDataExchange*>>::iterator iter;
    for (iter = mProducedData.begin(); iter != mProducedData.end(); ++iter)
    {
        FbsfDataExchange* iDataRef = (*iter).second;// public data
        if (iDataRef->Class() ==cScalar)
        {
            if (iDataRef->Type()==cInteger)
                iDataRef->setIntValue(*(int*)((*iter).first));
            else
                iDataRef->setRealValue(*(float*)((*iter).first));
        }
        else // Vector
        {
            if (iDataRef->Type()==cInteger)
            {
                QVector<int>* pVector =(QVector<int>*)(*iter).first;
                iDataRef->setData(pVector);
            }
            else
            {
                QVector<real>* pVector =(QVector<real>*)(*iter).first;
                iDataRef->setData(pVector);
            }
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// State management implementation
/// Internal save/restore
///     state values are registered in a local mStateDataMap (type,class,address)
///     the local map reference pointer to state data
/// File save/restore
///     the local map is copied to a QVariantMap with values
///     the copy is serialized/deserialized to/from file (FbsfExecutive)
///     The local state data are updated from QVariantMap via local map
/// TODO : check for uninitialized state data (not in temporary map)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::doSaveState
///        Executed when code request dump state to memory
/// \return 1 success 0 failed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::doSaveState()
{
    doTransition(cPreSaveState);
    // save states data to memory map
    for (QMap<QString, StateData>::iterator i = mStateDataRefMap.begin();
            i!= mStateDataRefMap.end();++i)
    {
        mStateDataValueMap[i.key()]=i.value().dataVariant();
    }
    doTransition(cPostSaveState);
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::doSaveState
///        Executed when code request reload state from memory
/// \return 1 success 0 failed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::doRestoreState()
{
    doTransition(cPreRestoreState);
    if(mStateDataValueMap.size()==0) return 0; // empty storage
    // restore states data to memory map
    for (QVariantMap::iterator i = mStateDataValueMap.begin();
            i!= mStateDataValueMap.end();++i)
    {
        if (mStateDataRefMap.contains(i.key()))
            mStateDataRefMap[i.key()]=mStateDataValueMap[i.key()];
        //else Internal state data is unreferenced
    }
    doTransition(cPostRestoreState);
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::doSaveState
///        Executed when code or application request dump state to memory or file
/// \param out : out stream (IODevice is QByteArray or QFile)
/// \return 1 success 0 failed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::doSaveState(QDataStream& out)
{
    // save states data to storageMap
    FBSFBaseModel::doSaveState();
    out << name();
    out << mSimulationTime;
    out << mStateDataValueMap;
    #ifdef TRACE
    if(!mStateDataValueMap.isEmpty())
    {
        qDebug()<< "FBSFBaseModel : " << name() <<"Save state";
        qDebug()<< mStateDataRefMap;
    }
    #endif

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Executed when application request dump state to file
/// \brief FBSFBaseModel::doSaveState
///        Executed when application request reload state from file
/// \param in : in stream
/// \return 1 success 0 failed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::doRestoreState(QDataStream& in)
{
    // restore states data to storageMap
    in >> mSimulationTime;
    in >> mStateDataValueMap;
    FBSFBaseModel::doRestoreState();

    postStep();// push exporters to the public data pool
    #ifdef TRACE
    if(!mStateDataValueMap.isEmpty())
    {
        qDebug()<< "FBSFBaseModel : " << name() <<"Restore state";
        qDebug()<< "Initialized" << mStateDataValueMap.size() << "of" << mStateDataRefMap.size();
        qDebug()<< mStateDataRefMap;
    }
    #endif
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VISUAL DOCUMENTS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::loadVisualDocument
///        method to manage visual documents
/// \param aDocument
/// \return
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::loadVisualDocument(QString aDocument)
{
    // get the path of the logic engine and visualization tool
    QUrl visualizer(QString("qrc:/GraphicEditor"));

    // get the path of the logic document and if the visualizer should be visible
    QUrl document=QUrl::fromLocalFile(aDocument);
    bool visibility = true;
    visibility = !(config().value("visibility")=="false");
    qInfo() << "\t\t" << "document :" << config()["document"] << " Visibility :"<< visibility ;

    QFileInfo check_file(aDocument);
    // check if file exists and if yes: Is it really a file and no directory?
    if(!check_file.exists())
        {
        QString msg("Can't open document file " + config()["document"] );
//#ifndef MODE_BATCH
//        QMessageBox::critical(nullptr, "[Fatal]", msg.toStdString().c_str());
//#endif
        qFatal(msg.toStdString().c_str());return 0;
    }
    // get the Application toolbar object
    QObject *appToolbar= rootWindow()->findChild<QObject*>("AppToolbar");
    QMetaObject::invokeMethod(appToolbar,"load",
                              Q_ARG(QVariant, name()),      // module instance name
                              Q_ARG(QVariant, visualizer),  // module type path
                              Q_ARG(QVariant, document),   // model document
                              Q_ARG(QVariant, visibility)); // hide or not the visualizer

    // get the plugin loaded document
    mQmlPlugin = rootWindow()->findChild<QObject*>(name());

    // handle error case
    if (!mQmlPlugin) {qFatal("Plugin Visualizer not connected");return 0;}

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::initializeVisualDocument
///        method to initialize visual documents
/// \return
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::initializeVisualDocument()
{
    // Initialize the model document
    int pastsize = AppConfig()["pastsize"].toInt();
    int futursize = AppConfig()["futursize"].toInt();
    int timeshift = AppConfig()["timeshift"].toInt();

    QMetaObject::invokeMethod(mQmlPlugin, "initialize",
                              Q_ARG(QVariant, mTimeStep),
                              Q_ARG(QVariant, pastsize),
                              Q_ARG(QVariant, futursize),
                              Q_ARG(QVariant, timeshift)
                              );

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::computeVisualDocument
///        method to compute visual documents
/// \return
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::computeVisualDocument()
{
    QMetaObject::invokeMethod(mQmlPlugin, "compute",
                              Qt::BlockingQueuedConnection,
                              Q_ARG(QVariant,mSimulationTime+mTimeStep));
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::saveStateVisualDocument
///        method to save states of visual documents
/// \return
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::saveStateVisualDocument()
{
    QMetaObject::invokeMethod(mQmlPlugin, "saveState",
                              Qt::BlockingQueuedConnection);
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::restoreStateVisualDocument
///        method to restore states of visual documents
/// \return
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::restoreStateVisualDocument()
{
    QMetaObject::invokeMethod(mQmlPlugin, "restoreState",
                              Qt::BlockingQueuedConnection);
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::doSaveState
/// Executed when application request dump states to file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::saveStateVisualDocument(QDataStream& out)
{
    // save the module states data to storage map
    FBSFBaseModel::doSaveState();
    // save the document states data to storage map
    QVariant states;
    QMetaObject::invokeMethod(mQmlPlugin, "getState",
                              Q_RETURN_ARG(QVariant, states));
    mStateDataValueMap["Document"]=states.toMap();
    // serialize map to file
    out << name();
    out << simulationTime();
    out << mStateDataValueMap;

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::doRestoreState
/// Executed when application request reload states from files
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModel::restoreStateVisualDocument(QDataStream& in)
{
    in >> mSimulationTime;
    in >> mStateDataValueMap;

    QVariantMap stateMap=mStateDataValueMap.take("Document").toMap();

    QMetaObject::invokeMethod(mQmlPlugin, "setState",
                              Q_ARG(QVariant,mSimulationTime),
                              Q_ARG(QVariant, QVariant::fromValue(stateMap) ));
    FBSFBaseModel::doRestoreState();
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::notifyParamChanged
/// Notification from GUI
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::notifyParamChanged(QString aName, QVariant aValue)
{
    Q_UNUSED(aName);
    Q_UNUSED(aValue);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// PUBLISH/SUBSCRIBE
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// publish local data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::publish(QString aName, int *aAddress,
                            QString aUnit, QString aDescription,
                            bool aHistory)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cInteger,
                                                FbsfDataExchange::cExporter,
                                                name(),aUnit,aDescription,
                                                cScalar,1,
                                                0,0,aHistory);
    if (publicAddress)
    {
        mProducedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        state(aName,aAddress); // register state data
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// publish local data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::publish(QString aName, float *aAddress,
                            QString aUnit, QString aDescription,
                            bool aHistory)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cReal,
                                                FbsfDataExchange::cExporter,
                                                name(),aUnit,aDescription,
                                                cScalar,1,
                                                0,0,aHistory);
    if (publicAddress)
    {
        mProducedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        state(aName,aAddress); // register state data
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// publish QVector<int> data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::publish(QString aName, QVector<int>* aVector,
                            QString aUnit, QString aDescription,
                            int aTimeShift, int aIndex,bool aHistory)
{
    // Check if empty vector
    if (aVector->size()==0) qCritical() << "Publishing null size vector :" << aName;
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cInteger,
                                                FbsfDataExchange::cExporter,
                                                name(),aUnit,aDescription,
                                                cVector,aVector->size(),
                                                aTimeShift,aIndex,aHistory);
    if (publicAddress)
    {
        mProducedData.append(QPair<void*,FbsfDataExchange*>(aVector,publicAddress));
        state(aName,aVector); // register state data
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// publish QVector<float> data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::publish(QString aName, QVector<real>* aVector,
                            QString aUnit, QString aDescription,
                            int aTimeShift, int aIndex,bool aHistory)
{
    if (aVector->size()==0) qCritical() << "Publishing null size vector :" << aName;
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cReal,
                                                FbsfDataExchange::cExporter,
                                                name(),aUnit,aDescription,
                                                cVector,aVector->size(),
                                                aTimeShift,aIndex,aHistory);
    if (publicAddress)
    {
        mProducedData.append(QPair<void*,FbsfDataExchange*>(aVector,publicAddress));
        state(aName,aVector); // register state data
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// publish application type data to plublic data pool
/// aType : application type (parameters, constants)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::publishAppType(FbsfAppType aType,
                              QString aName, int *aAddress,
                              QString aUnit, QString aDescription,
                              bool aHistory)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cInteger,
                                                FbsfDataExchange::cExporter,
                                                name(),aUnit,aDescription,
                                                cScalar,1,
                                                0,0,aHistory);
    if (publicAddress)
    {
        mProducedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        publicAddress->value(*aAddress); // push initial value in the plublic data pool
        publicAddress->appType(aType);  // set application type
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// publish application type data to plublic data pool
/// aType : application type (parameters, constants)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::publishAppType(FbsfAppType aType,
                              QString aName, float *aAddress,
                              QString aUnit, QString aDescription,
                              bool aHistory)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cReal,
                                                FbsfDataExchange::cExporter,
                                                name(),aUnit,aDescription,
                                                cScalar,1,
                                                0,0,aHistory);
    if (publicAddress)
    {
        mProducedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        publicAddress->value(*aAddress); // push initial value in the plublic data pool
        publicAddress->appType(aType);  // set application type
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// subscribe int data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::subscribe(QString aName,int *aAddress,
                              QString aUnit, QString aDescription)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cInteger,
                                                FbsfDataExchange::cImporter,
                                                name(),aUnit,aDescription);
    if (publicAddress)
    {
        mConsumedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        publicAddress->value(*aAddress); // initial value in plublic data pool
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// subscribe float data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::subscribe(QString aName, float *aAddress,
                              QString aUnit, QString aDescription)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cReal,
                                                FbsfDataExchange::cImporter,
                                                name(),aUnit,aDescription);
    if (publicAddress)
    {
        mConsumedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        publicAddress->value(*aAddress); // initial value in plublic data pool
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// subscribe QVector<int> to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::subscribe(QString aName,QVector<int>* aVector,
                              QString aUnit, QString aDescription)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cInteger,
                                                FbsfDataExchange::cImporter,
                                                name(),aUnit,aDescription,
                                                cVector,aVector->size());
    if (publicAddress)
        mConsumedData.append(QPair<void*,FbsfDataExchange*>(aVector,publicAddress));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// subscribe QVector<real> to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::subscribe(QString aName,QVector<real>* aVector,
                              QString aUnit, QString aDescription)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cReal,
                                                FbsfDataExchange::cImporter,
                                                name(),aUnit,aDescription,
                                                cVector,aVector->size());
    if (publicAddress)
        mConsumedData.append(QPair<void*,FbsfDataExchange*>(aVector,publicAddress));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// subscribe int data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::subscribeAppType(FbsfAppType aType,QString aName, int *aAddress,
                                     QString aUnit, QString aDescription)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cInteger,
                                                FbsfDataExchange::cImporter,
                                                name(),aUnit,aDescription);
    if (publicAddress)
    {
        mConsumedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        publicAddress->value(*aAddress); // push initial value in the plublic data pool
        publicAddress->appType(aType);   // set application type
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// subscribe float data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::subscribeAppType(FbsfAppType aType,QString aName, float *aAddress,
                                     QString aUnit, QString aDescription)
{
    FbsfDataExchange* publicAddress =
            FbsfDataExchange::declarePublicData(aName,
                                                cReal,
                                                FbsfDataExchange::cImporter,
                                                name(),aUnit,aDescription);
    if (publicAddress)
    {
        mConsumedData.append(QPair<void*,FbsfDataExchange*>(aAddress,publicAddress));
        publicAddress->value(*aAddress); // push initial value in the plublic data pool
        publicAddress->appType(aType);   // set application type
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// push value data to plublic data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModel::publishValue(QString aName, int aValue)
{
    FbsfDataExchange *publicAddress = FbsfDataExchange::sPublicDataMap.value(aName);
    if (publicAddress!=nullptr)
         publicAddress->value(aValue);
    FbsfDataExchange::updateDataModel();
}
void FBSFBaseModel::publishValue(QString aName,real aValue)
{
    FbsfDataExchange *publicAddress = FbsfDataExchange::sPublicDataMap.value(aName);
    if (publicAddress!=nullptr)
         publicAddress->value(aValue);
    FbsfDataExchange::updateDataModel();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::state : State declaration for int
/// \param aName : variable name
/// \param aVector : data pointer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void    FBSFBaseModel::state(QString aName,int *aAddress)
{
    mStateDataRefMap.insert(aName,StateData(cInteger,cScalar,aAddress));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::state : State declaration for real
/// \param aName : variable name
/// \param aVector : data pointer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void    FBSFBaseModel::state(QString aName,real *aAddress)
{
    mStateDataRefMap.insert(aName,StateData(cReal,cScalar,aAddress));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::state : State declaration for QVector<int>
/// \param aName : variable name
/// \param aVector : data pointer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void    FBSFBaseModel::state(QString aName,QVector<int>* aVector)
{
    mStateDataRefMap.insert(aName,StateData(cInteger,cVector,aVector));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \brief FBSFBaseModel::state : State declaration for QVector<real>
/// \param aName : variable name
/// \param aVector : data pointer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void    FBSFBaseModel::state(QString aName, QVector<real> *aVector)
{
    mStateDataRefMap.insert(aName,StateData(cReal,cVector,aVector));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Performance meter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FBSFBaseModel::getPerfMeterInitial()
{
    // declare export
//    publish(name()+":CpuStepTime", &mCpuStepTime,
//            "ms","Step time");
    // declare export
    mCpuStepTime =
        FbsfDataExchange::declarePublicData(name()+":CpuStepTime",
                                            cInteger,
                                            FbsfDataExchange::cExporter,
                                            name(),"ms","Step time");
    // return initalization time
    return  QString::number(mCpuInitializationTime);
}
QString FBSFBaseModel::getPerfMeterFinal()
{
    return  QString::number(mCpuFinilizationTime);
}
QString FBSFBaseModel::getPerfMeterStep()
{
    return QString::number(mCpuStepTime->getIntValue());
}
void  FBSFBaseModel::resetCpuTime()
{
    if(mCpuStepTime!=nullptr)
        mCpuStepTime->setIntValue(0);
}
//~~~~~~~~~~~~~~~~~~ Debug helper ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QDebug operator<<(QDebug dbg, const FBSFBaseModel::StateData &data)
{
    dbg << "(" << (char)data.mDataType << (char)data.mDataClass << ")";
    if (data.mDataType==cInteger)
    {
        if(data.mDataClass==cScalar)
            dbg <<*(static_cast<int*>( data.pData) );
        else
            dbg << *(static_cast<QVector<int>*>( data.pData) );
    }
    else//cReal
    {
        if(data.mDataClass==cScalar)
            dbg << *(static_cast<real*>( data.pData) );
        else
            dbg << *(static_cast<QVector<real>*>( data.pData) );
    }
    return dbg;
}

