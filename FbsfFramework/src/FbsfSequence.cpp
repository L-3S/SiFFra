#include "FbsfApplication.h"
#include "FbsfExecutive.h"
#include "FbsfSequence.h"
#include "FbsfBaseModel.h"
#include "FbsfPublicData.h"

#include "FbsfNode.h"
#include <QThread>
#include <iostream>
#include <QMap>
#include <QList>
#define SYNCHRONE_INIT  // sequential mode

//#define TRACE

///TODO : use error signal to common slot in executive
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \class FbsfSequence
/// \brief Sequence controller for models
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSequence::FbsfSequence(QString aName, float aPeriod, FbsfApplication *app, QSemaphore   &ptaskCond, QSemaphore   &piterCond)
    : mName(aName)
    , mPeriod(aPeriod)
    , mIter(1)
    , mRemainIter(0)
    , mStepNumber(0)
    , mApp (app)
    , mStatus(FBSF_OK)
    , taskCond(ptaskCond)
    , iterCond(piterCond)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSequence::~FbsfSequence()
{
    emit finished();// terminate control thread
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Registration of models
/// Each model is executed in the sequence thread
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfSequence::addModels(QList<QMap<QString, QString> > &aModels, QList<FbsfConfigNode> & aNodes)
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Instanciation of model(s) types : Fmu,Visual or Manual
    // Register the model to the executive system
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // default timeStep = application timeStep (timer period)
    float   timeStep = mApp->config().Simulation()["timestep"].toFloat()*mPeriod;
    // set the iteration number if FAST iteration
    if(mPeriod < 1) mIter=1/mPeriod;

    qInfo() << "Sequence : " << name()
            << "period :"    << mPeriod
            << "timestep :"  << timeStep ;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get the models parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QList<QMap<QString,QString>>::const_iterator iMod;
    for (iMod = aModels.begin(); iMod != aModels.end(); ++iMod)
    {

        QString modelType;
        QString modelName;
        QString modelPath;

        bool    dumpCsv        = false;
        bool    typeFmu        = false;
        bool    typeVisual     = false;
        bool    typeNode       = false;
        float   version        = 1.0;

        double  startTime      = 0.0;
        double  stopTime       = DBL_MAX ;

        QMap<QString,QString> vMap = *iMod;
        foreach ( const QString& key, vMap.keys())
        {
            QString value=vMap.value(key);

            if (key == "module")        modelType=value;
            else if (key == "name")     modelName=value;
            else if (key == "path")     modelPath=value;
            else if (key == "type")
            {
                typeNode    = (value=="node");
                typeFmu     =(value=="fmu");
                typeVisual  =(value=="visual");
            }
            else if (key == "version")   version= value.toFloat();
            else if (key == "dumpcsv")   dumpCsv =(value=="true");
            else if (key == "starttime") startTime=value.toDouble();
            else if (key == "stoptime")  stopTime =value.toDouble();
            else if (key == "timestep")  timeStep=value.toFloat();
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        FBSFBaseModel* model;
        if (typeNode)
        {
            static int n = 0;
            n++;
            if (aNodes.size() < 1)
                return 0;
            FbsfNode *node = new FbsfNode();
            node->name(modelName); 
            for(int i = 0; i < aNodes[0].Sequences().size(); i++)
            {
                node->addSequence("SubSeq" + QString::number(n*10+i), 1,
                                  aNodes[0].Sequences()[i].Nodes() ,
                                  aNodes[0].Sequences()[i].Models(), mApp);
            }
            aNodes.pop_front();
            model = node;

        } else if (typeFmu) // FMU model
        {
             model = FBSFBaseModel::loadFMUModel(modelName,modelPath,timeStep,
                                                 startTime,stopTime,
                                                 dumpCsv);
            if (model==nullptr) return 0;
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        else if (typeVisual) // model working as a QML controler
        {   ///TODO check visual model with server mode
#ifdef MODE_BATCH
            qFatal("Visual module not allowed for batch mode");
            return 0;
#else
            model=FBSFBaseModel::loadModule(modelType,modelName,timeStep,
                                            mApp->getRootWindow());
            if (model==nullptr) return 0;
#endif

        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        else // Manual
        {
            model=FBSFBaseModel::loadModule(modelType,modelName,timeStep);
            if (model==nullptr) return 0;

        }
        if (model->getParamList().size() > 0) { // Check parameters
            FbsfConfiguration::CheckValidity(vMap, model->getParamList());
        }
        // setup configuration of the module
        model->config(vMap);
        // setup global configuration
        model->AppConfig(mApp->config().Simulation());
        addModel(model);

    }// end Models
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Register model
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::addModel(FBSFBaseModel* aModel)
{
    mModelList.append(aModel);// register model to sequence
    mApp->executive()->modelMap().insert(aModel->name(),aModel);// register model to app

    // batch mode enable signal to control simulation
    QObject::connect((QObject *)aModel,SIGNAL(ExecutiveControl(QString,QString)),
                     mApp->executive(), SLOT(control(QString,QString)),
                     Qt::DirectConnection);
    connect(this, SIGNAL(cancelModelStep()), aModel, SLOT(cancelStep()));

    #ifdef TRACE
        qDebug() << "FbsfSequence::addModel : " << aModel->name();
    #endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::initialize()
{
    // perf meter
    QElapsedTimer timer;
    timer.start();

    #ifdef TRACE
    qDebug() << "FbsfSequence : " << name()
             <<"initialized in thread: " << QThread::currentThreadId();
    #endif
    // for each model do mStatus = doInit();
    for (int i = 0; i < mModelList.size(); ++i)
    {
       mModelList[i]->initialize();
       if(mModelList[i]->status()==FBSF_ERROR)
       {
            // signal error to the executive system
            error("module " + mModelList[i]->name()
                 +" initialization failed");
       }
    }
    // perf meter
    mCpuInitializationTime=timer.elapsed();
    #ifndef SYNCHRONE_INIT
    signalCompletion();
    #endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::finalize()
{
    // perf meter
    QElapsedTimer timer;
    timer.start();

    // for each model do mStatus = doTerminate();
    for (int i = 0; i < mModelList.size(); ++i)
    {
        mModelList[i]->finalize();
        if(mModelList[i]->status()==FBSF_ERROR)
        {
             // signal error to the executive system
             error("module " + mModelList[i]->name()
                  +" finalization failed");
        }
    }
    // perf meter
    mCpuFinilizationTime=timer.elapsed();

    #ifdef TRACE
    qDebug() << "FbsfSequence : " << name()
             << "terminated in thread: "<<QThread::currentThreadId();
    #endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// reset the sequence iteration count
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void  FbsfSequence::cycleStart()
{
    mRemainIter=mIter;
    for (int i = 0; i < mModelList.size(); ++i)
        mModelList[i]->resetStepRunning();
}
void FbsfSequence::cancelSeqStep() {
    emit cancelModelStep();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Data consumption procedure : signal to virtual method call
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::consumeData()
{

#ifdef SYNCHRONE_INPUT
    if (mRemainIter)
    {
        /// for models do preStep();
        for (int i = 0; i < mModelList.size(); ++i)
        {
            mModelList[i]->consumeData();
            mStatus=mModelList[i]->status();
        }
    }
#endif
    signalCompletion();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Step computation procedure : signal virtual method call
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::computeStep(int timeOut)
{
    // perf meter
    QElapsedTimer timer;
    timer.start();
    mStatus = FBSF_OK;
    // reset module cpu time to avoid persistance
    for (int i = 0; i < mModelList.size(); i++)mModelList[i]->resetCpuTime();

    if (mPeriod >1) // SLOW iteration
    {
        if (mStepNumber%(int)mPeriod==0)
        {
            // for each models do preStep(), doStep() incuding postStep();
            for (int i = 0; i < mModelList.size(); i++)
            {
                #ifndef SYNCHRONE_INPUT
                mModelList[i]->consumeData();
                #endif
                mModelList[i]->computeStep(timeOut);
                if (mStatus == FBSF_OK) {
                    mStatus=(fbsfStatus)mModelList[i]->status();
                }
            }
        }
        iterCond.acquire();// decrease the working count
    }
    else
    // NORMAL or FAST iteration : process all iterations
    if (mRemainIter)
    {
        // for each models do preStep(), doStep() incuding postStep();;
        for (int i = 0; i < mModelList.size(); i++)
        {
            #ifndef SYNCHRONE_INPUT
            mModelList[i]->consumeData();
            #endif
            mModelList[i]->computeStep(timeOut);
            if (mStatus == FBSF_OK) {
                mStatus=(fbsfStatus)mModelList[i]->status();
            }
            if(mModelList[i]->status()==FBSF_ERROR)
            {
                 // signal error to the executive system
                 error("module " + mModelList[i]->name()
                      +" computation failed");
            }
        }
        // iter count down
        mRemainIter--;

        if (mRemainIter==0)
                {iterCond.acquire();}// decrease the working count
    }
    // perf meter
    if(mCpuStepTime!=nullptr)
        mCpuStepTime->setIntValue(timer.elapsed());

    mStepNumber++;
    signalCompletion();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Model state serialization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfSequence::doSaveState(QDataStream& out)
{
    // for each model do doSaveState();
    for (int i = 0; i < mModelList.size(); ++i)
    {
        mModelList[i]->doSaveState(out);
        mStatus=(fbsfStatus)mModelList[i]->status();
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Model state deserialization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfSequence::doRestoreState(QDataStream& in)
{
    // for each model do doRestoreState();
    for (int i = 0; i < mModelList.size(); ++i)
    {
        mModelList[i]->doRestoreState(in);
        mStatus=(fbsfStatus)mModelList[i]->status();
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Model state serialization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfSequence::doSaveState()
{
    int operationStatus = FBSF_OK;
    mStatus = FBSF_OK;
    // for each model do doSaveState();
    for (int i = 0; i < mModelList.size(); ++i)
    {
        operationStatus = mModelList[i]->doSaveState();
        if (mStatus == FBSF_OK) {
            mStatus=(fbsfStatus)operationStatus;
        }
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Model state deserialization
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfSequence::doRestoreState()
{
    int operationStatus = FBSF_OK;
    mStatus = FBSF_OK;
    // for each model do doRestoreState();
    for (int i = 0; i < mModelList.size(); ++i)
    {
        operationStatus = mModelList[i]->doRestoreState();
        if (mStatus == FBSF_OK) {
            mStatus=(fbsfStatus)operationStatus;
        }
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// static Method for executive controler synchronization
/// on task completion
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::waitCompletion(int aNbTasks)
{
    taskCond.acquire(aNbTasks);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// signal completion to Executive controller
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::signalCompletion()
{
    taskCond.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// reset number of working called from Executive controller
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSequence::resetWorking(int aNbSequences)
{
    iterCond.release(aNbSequences);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// number of working with iterations called
/// from Executive controller
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfSequence::stillWorking()
{
    return iterCond.available();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Performance meter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfSequence::getPerfMeterInitial()
{
    // declare export
    mCpuStepTime =
        FbsfDataExchange::declarePublicData(name()+":CpuStepTime",
                                            cInteger,
                                            FbsfDataExchange::cExporter,
                                            name(),"ms","Step time");
    // return initalization time as a string
    return QString::number(mCpuInitializationTime);
}
QString FbsfSequence::getPerfMeterFinal()
{
    return QString::number(mCpuFinilizationTime);
}
QString FbsfSequence::getPerfMeterStep()
{
    return QString::number(mCpuStepTime->getIntValue()) ;
}
