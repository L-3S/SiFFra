
#include "FbsfExecutive.h"
#include "FbsfApplication.h"

#include "FbsfSequence.h"
#include "FbsfControler.h"
#include "FbsfBaseModel.h"
#include "FbsfPublicData.h"
#include "FbsfNetServer.h"
#include "FbsfNetClient.h"
#include "FbsfNetLogger.h"
#include "FbsfPerfmeter.h"

#include <QThread>
#include <QString>
#include <QDebug>
#include <QHostInfo>
#include <QElapsedTimer>

#ifndef MODE_BATCH
#include <QtWidgets/QMessageBox>
#endif

#define SYNCHRONE_INIT      // Trigger input on cycle start
using namespace FbsfNetLogger;

FbsfNetClient* FbsfExecutive::mNetClient=nullptr;
FbsfNetServer* FbsfExecutive::mNetServer=nullptr;
bool           FbsfExecutive::sOptPerfMeter=false;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \class FbsfExecutive
/// \brief Executive system
/// Timer for control computation loop (default 100 ms)
/// Control the simulation states and modes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExecutive::FbsfExecutive(eApplicationMode aMode)
    : mCycleTime(100)
    , mPeriod (100)
    , mSimulationTime(0)
    , mStepNumber(0)
    , mReplayMode(false)
    , mReplayFile(QString())
    , workflowState( ePause )
    , mAppMode(aMode)
    , mExeMode(eCompute)
    , mReplayLength(0)
    , bComputeTime(true)
    , bSuspended(false)
    , mStatus(FBSF_OK)
    , mMultiSteps(INFINITE)
    , bExitAfterSteps(false)

{
    if (mAppMode == client)
    {
        QString serverHost = qgetenv("FBSF_SERVER");
        if (serverHost.isEmpty()) qFatal("Environment variable FBSF_SERVER not set");
        mNetClient= new FbsfNetClient(QHostInfo::localHostName());
        mDataControler.setClientMode(mNetClient);
        mNetClient->connectServer(serverHost);
        if (!mNetClient->status())
        {
            throw "Connection to server failed";
        }
    }
    else if (mAppMode == server)
    {
        mNetServer = new FbsfNetServer();
        // Slot for executive control from remote client
        connect(mNetServer,SIGNAL(executiveMsg(QString)),this, SLOT(control(QString)));
        mDataControler.setServerMode(mNetServer);
    }
    if (mAppMode == client || mAppMode == server)
    {
        FbsfNetLoggerManager *manager = FbsfNetLoggerManager::getInstance();

        QStringList moduleList;
        moduleList.append("NetServer");
        moduleList.append("NetClientThread");
        moduleList.append("NetClient");
        manager->addDestination(QDir::currentPath() + "/logs/server.log",
                                QStringList("NetServer"),FbsfNetLogger::TraceLevel);
        manager->addDestination(QDir::currentPath() + "/logs/server.log",
                                QStringList("NetSrvClient"),FbsfNetLogger::TraceLevel);
        manager->addDestination(QDir::currentPath() + "/logs/client.log",
                                QStringList("NetClient"),FbsfNetLogger::TraceLevel);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExecutive::~FbsfExecutive()
{
    if (mNetClient) delete mNetClient;
    if (mNetServer) delete mNetServer;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// set the replay mode
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void  FbsfExecutive::ReplayMode(bool aFlag, QString aFile)
{
    qDebug() << "Replay simulation from : " << aFile;

    mReplayLength = FbsfDataExchange::readDataFromFile(aFile);
    mReplayMode=aFlag;

    if(aFlag && FbsfDataExchange::ReplayFull())// replay mode
        {mExeMode=eFullReplay;} // no produced data
    else
        {mExeMode=ePartialReplay;}// still any produced data

    qDebug()<< "Replay Mode : " << ExecutionMode()
            << "Duration :" << mReplayLength << "steps";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// set the batch mode
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void  FbsfExecutive::BatchMode(bool aFlag)
{
    mExeMode=eBatch;
    workflowState=eRun;
    Speed(mFastSpeedFactor);

    qDebug()<< "Mode : " << ExecutionMode();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Registration of sequences
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfExecutive::addSequence(QString aName, float aPeriod,
                               QList<FbsfConfigNode>& aNodes,
                               QList<QMap<QString, QString> > &aModels,
                               FbsfApplication *app)
{
    FbsfSequence* pSequence=new FbsfSequence(aName,aPeriod,app, taskCond, iterCond);
    pSequence->addModels(aModels, aNodes);
    // Create the sequence thread
    QThread* thread = new QThread;
    pSequence->moveToThread(thread);

    // connect signal to sequence slots : cycle start
    connect(this  , SIGNAL(cycleStart()) , pSequence, SLOT(cycleStart()));
    std::cout << "setup seq STEP" << this << std::endl;
    connect(this  , SIGNAL(cancelStep()) , pSequence, SLOT(cancelSeqStep()));
    connect(this  , SIGNAL(cancelStep()) , thread, SLOT(quit()));
    // connect signal to sequence slots : pre/step computation and finalization
    connect(this  , SIGNAL(consume()) , pSequence, SLOT(consumeData()));
    connect(this  , SIGNAL(compute()) , pSequence, SLOT(computeStep()));
    // release
    connect(pSequence, SIGNAL(finished()), thread, SLOT(quit()));
    connect(thread   , SIGNAL(finished()), thread, SLOT(terminate()));
    // Message collector
    connect(pSequence, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

#ifdef SYNCHRONE_INIT
    // due to QML monitoring (FbsfDataExchange must belong to this thread)
    pSequence->initialize();
#else
    // synchronize initialisation on thread started event
    connect(thread, SIGNAL(started()) , pSequence, SLOT(initialize()));
#endif
    // start thread and wait for initialisation completion
    thread->start();
#ifndef SYNCHRONE_INIT
    FbsfSequence::waitCompletion(1);// Asynchronous initialisation
#endif
    if(pSequence->status())
        mSequenceList.append(pSequence);// register sequence
    else
        thread->terminate(); // failure
    return pSequence->status();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Executive loop for model computation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExecutive::run()
{
    QTime stepTime;
    QTime batchTime;
    int   totalSteps=0;

    // set the perfmeter
    FbsfPerfMeter* perfmeter=nullptr;
    if(sOptPerfMeter)
    {
        perfmeter=new FbsfPerfMeter(this);
        QString vPerfFile = QDir::currentPath() + "/PerfMeter.csv";
        perfmeter->openFile(vPerfFile);
        // dump the perf info to file
        perfmeter->DumpToFile(mStepNumber, FbsfPerfMeter::cInitial);
    }

    // Compute Simulation Time only if Server,
    // Standalone modes and not already produced
    if (mAppMode == client || FbsfDataExchange::isPublished("Simulation.Time"))
    {
        bComputeTime=false;
    }
    else
    {
        mPublicSimulationTime =
            FbsfDataExchange::declarePublicData("Simulation.Time",
                                                cReal,
                                                FbsfDataExchange::cExporter,
                                                "FbsfExecutive","s","Simulation Time");
    }
    mDataControler.initializePublicDataList();
    FbsfDataExchange::recorderSize(Recorder());// set the maximum recording size

    if (mStatus==FBSF_ERROR) emit statusChanged(ExecutionMode(),"error");

    emit statusChanged(ExecutionMode(),"initialized");// Update UI with the initial mode/state
    // start simulation loop
    stepTime.start();
    if (BatchMode()) batchTime.start();
    while (workflowState!=eStop)
    {
        // check state change command
        if (workflowState==ePause)
        {
            emit statusChanged(ExecutionMode(),State()); mPauseCond.acquire(1);
        }
        if (workflowState==eStop) break; // break loop

        // reset step compute time
        stepTime.restart();
        // run major cycle
        emit statusChanged(ExecutionMode(),"computing");
        doCycle();
        // check state change command
        if (workflowState==eStep)
        {
            workflowState = ePause;
            continue; // return to Pause state
        }
        else if (workflowState==eRun && mMultiSteps != INFINITE)
        {
            mMultiSteps--;
            if(mMultiSteps==0)
            {
                if(bExitAfterSteps)
                    workflowState = eStop;
                else
                    workflowState = ePause;
                continue; // return to Pause state
            }
        }
        else if (workflowState==eStop) break; // break loop

        totalSteps+=1;

        // dump the perf info to file
        if(sOptPerfMeter && perfmeter!=nullptr)
            perfmeter->DumpToFile(mStepNumber, FbsfPerfMeter::cStep);

        // Compute suspend delay
        if (stepTime.elapsed() < mCycleTime) //real time checking
        {
            if(mCycleTime - stepTime.elapsed() < 10 && !BatchMode())
            {
                suspend(10);// time guard
            }
            else
            {
                emit statusChanged(ExecutionMode(),"waiting");
                suspend(mCycleTime - stepTime.elapsed());// suspend for realtime
            }
        }
        else if (!BatchMode()) suspend(10);// time guard
    }
    for(int s=0;s<mSequenceList.size();s++) mSequenceList[s]->finalize();

    // dump the perf info to file
    if(sOptPerfMeter && perfmeter!=nullptr)
        perfmeter->DumpToFile(mStepNumber, FbsfPerfMeter::cFinal);

    if (BatchMode())
    {
        qDebug() << "Batch total steps        :" << mStepNumber ;
        qDebug() << "Batch Simulated time     :" << Period()*(mStepNumber-1) << "ms";
        qDebug() << "Batch mode expected time :" << mCycleTime*(mStepNumber-1) << "ms";
        qDebug() << "Batch mode elapsed time  :" << batchTime.elapsed() << "ms";
        qDebug() << "Defined time step        :" << Period() << "ms";
        qDebug() << "Effective Cycle time     :" << mCycleTime << "ms";
    }
    if(sOptPerfMeter){
        qDebug() << "Dumped PerfMeter.csv file to" << QDir::currentPath();
        delete perfmeter;
    }
    emit exit();// exit from thread
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SLOT : Control of the executive workflow
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExecutive::doCycle()
{
    QElapsedTimer stepDuration;
    stepDuration.start();
    resetWorking(mSequenceList.size());
    emit cycleStart();
    // loop until all iterations completed for all sequences
    while (stillWorking()>0)
    {
        emit consume();// Synchronous consumption of inputs
        waitCompletion(mSequenceList.size());
        emit compute();// compute models
        waitCompletion(mSequenceList.size());
    }
    // perf meter
    if(mCpuStepTime!=nullptr)
        mCpuStepTime->setIntValue(stepDuration.elapsed());

    if (bComputeTime)
    {
        // Compute and publish simulation time
        mSimulationTime+=mPeriod;
        if (mAppMode != client && mPublicSimulationTime)
            mPublicSimulationTime->setRealValue(mSimulationTime/(1000));
    }
    // broadcast public data values to clients
    mDataControler.process(mStepNumber);
    // signal status changed to plugins

    emit statusChanged(ExecutionMode(),"stepEnd");

    // process replay steps until end
    if (ReplayMode() && mStepNumber >= mReplayLength)
    {   // Update UI with the initial mode/state
        control("pause","");
    }
    else
    {
        mStepNumber++;// usefull for replay
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SLOT : Control of the executive workflow
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExecutive::control(QString command, QString param1, QString param2)
{
    // broadcast local command to ALL clients executive
    // then process the executive command locally
    if (mAppMode == client && mNetClient)
        mNetClient->sendExecutiveMsg(command);// TODO send param
    if (command == "cancel") {
        std::cout << "CANLING STEP" << this << std::endl;
        emit FbsfExecutive::cancelStep();
//        emit cycleStart();
        workflowState = ePause;
        if (isSuspended()) wakeup();
    //~~~~~~~~~~~~~~~~~~ Pause simulation ~~~~~~~~~~~~~~~~~~~~~~~
    } else if (command == "pause") {
        //~~~~~~~~~~~~~~~~~~ Pause simulation ~~~~~~~~~~~~~~~~~~~~~
        workflowState = ePause;
        if (isSuspended()) wakeup();
        //emit statusChanged(ExecutionMode(),State());
    }
    //~~~~~~~~~~~~~~~~~~ Back Track ~~~~~~~~~~~~~~~~~~~~~~~
    else if (command == "backtrack")
    {
        if (param1 == "on") {// GRO BackTrack
            FbsfDataExchange::switchBacktrack(true);
            FbsfDataExchange::updateBacktrack(param2.toInt());
        } else if (param1 == "off") {
            FbsfDataExchange::switchBacktrack(false);
        } else
        {
            FbsfDataExchange::updateBacktrack(param1.toInt());
        }
    }
    //~~~~~~~~~~~~~ Execute one step simulation ~~~~~~~~~~~~~~~
    else if (command == "step")
    {   // step and pause
        if (isSuspended()) wakeup();
        else mPauseCond.release();
        workflowState = eStep;
        emit statusChanged(ExecutionMode(),State());
    }
    //~~~~~~~~~~~~~~~~~~ Run simulation ~~~~~~~~~~~~~~~~~~~~~~~
    else if (command == "run")
    {
        if (param1.isEmpty())
            mMultiSteps=INFINITE;
        else
            mMultiSteps=param1.toInt();
        if (param2=="stop")
            bExitAfterSteps=true;
        else
            bExitAfterSteps=false;

        workflowState = eRun;
        Speed(1.0);// reset the speed factor
        mPauseCond.release();
        emit statusChanged(ExecutionMode(),State());
    }    //~~~~~~~~~~~~~~~~~Fast/Slow Run simulation ~~~~~~~~~~~~~~~~~~~~~
    else if (command == "speed")
    {
        bExitAfterSteps=false;
        if (param1.isEmpty())
            mMultiSteps=INFINITE;
        else
            mMultiSteps=param1.toUInt();
        if (param2=="stop")
            bExitAfterSteps=true;
        else
            bExitAfterSteps=false;
        Speed(mFastSpeedFactor);// set the speed factor
        if (isSuspended()) wakeup();
        else mPauseCond.release();
        workflowState = eRun;
        emit statusChanged(ExecutionMode(),State());
    }
    //~~~~~~~~~~~~~~~~~~~~ Save states ~~~~~~~~~~~~~~~~~~~~~~~~
    else if (command == "save" && mAppMode != client)
    {
        QFile snapFile(QUrl(param1).toLocalFile());
        if (!snapFile.open(QIODevice::WriteOnly))
        {
            QString msg("Could not write to snapshot file:"+ param1
                     + "\n" + snapFile.errorString());
#ifndef MODE_BATCH
            QMessageBox::critical( nullptr, "[Error]", msg);
#endif
            return;
        }
        QDataStream out(&snapFile);
        out.setVersion(QDataStream::Qt_5_7);
        out << static_cast<quint32>(FBSF_MAGIC);    // Magic number
        out << static_cast<quint32>(1);             // Format version
        out << QString(FBSF_VERSION);               // Framework version
        out << mSimulationTime;                     // Global simulation time
        for (int iSeq=0;iSeq < mSequenceList.size();iSeq++)
        {
            mSequenceList[iSeq]->doSaveState(out);
        }
        snapFile.close();
    }
    //~~~~~~~~~~~~~~~~~~~~ Restore states ~~~~~~~~~~~~~~~~~~~~~~~~
    else if (command == "restore" && mAppMode != client)
    {
        QFile snapFile(QUrl(param1).toLocalFile());
        if (!snapFile.open(QIODevice::ReadOnly))
        {
            QString msg("Could not read snapshot file:"+ param1
                     + "\n" + snapFile.errorString());
#ifndef MODE_BATCH
            QMessageBox::warning( nullptr, "[Error]", msg);
#endif
            return;
        }
        QDataStream in(&snapFile);in.setVersion(QDataStream::Qt_5_7);
        // Read and check the file header
        quint32 magic;
        in >> magic;
        if (magic != FBSF_MAGIC)
        {
            QString msg("Snapshot file " + param1 +" : " +
                        "bad file format");
#ifndef MODE_BATCH
            QMessageBox::warning( nullptr, "[Error]", msg.toStdString().c_str());
#endif
        }
        // Get the format version
        quint32 format;
        in >> format;
        // Get the framework version
        QString version;
        in >> version;
        // Get the global simulation time
        in >> mSimulationTime;
        // For each modules restore the states data
        while (!in.atEnd())
        {
            QString vName;
            in >> vName;    // find the module name to call restore
            if (mModelMap.contains(vName))
                mModelMap[vName]->doRestoreState(in);
            else
                FBSFBaseModel::consumeStateData(in);// consume stream data
        }
        snapFile.close();
        if (bComputeTime)
        {
            // Publish simulation time
            if (mAppMode != client && mPublicSimulationTime)
                mPublicSimulationTime->setRealValue(mSimulationTime/(1000));
        }
        //FbsfDataExchange::resetHistory(mSimulationTime);
        QString strTime;
        control("resettime",strTime.setNum(mSimulationTime));
#ifndef MODE_BATCH
        mDataControler.updateMonitor();// update only data values
#endif
        //mDataControler.process(mStepNumber);// broadcast values to clients
    }
    //~~~~~~~~~~~~~~~~~~ Rest simulation start time ~~~~~~~~~~~~~~~~~~~~~
    else if (command == "resettime")
    {   // simulation time in ms
        FbsfDataExchange::resetHistory(static_cast<float>(param1.toInt())/(1000));
        emit statusChanged(ExecutionMode(),"reseted");
    }
    //~~~~~~~~~~~~~~~~~~ Record simulation ~~~~~~~~~~~~~~~~~~~~~
    else if (command == "record")
    {
        FbsfDataExchange::writeDataToFile(param1);
    }
    //~~~~~~~~~~~~~~~~~~ Replay simulation ~~~~~~~~~~~~~~~~~~~~~
    else if (command == "replay")
    {
        mStepNumber = param1.toUInt();
        mDataControler.process(mStepNumber);
        mStepNumber++; // for next doCycle
    }
    //~~~~~~~~~~~~~~~~~~ Stop simulation ~~~~~~~~~~~~~~~~~~~~~
    else if (command == "stop")
    {
        workflowState = eStop;
        if (isSuspended()) wakeup();
        else mPauseCond.release();//exit from pause state
        emit statusChanged(ExecutionMode(),State());
    }
    else
        DEBUG << command << "unknown executive command";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Error message collector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExecutive::errorString(QString err)
{
        qDebug() << "[Error] : " <<  err;
        mStatus=FBSF_ERROR;
        emit statusChanged(ExecutionMode(),"error");
        control("pause","");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get execution mode as string
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfExecutive::ExecutionMode()
{
    switch(mExeMode)
    {
        case eCompute       : return "compute";
        case eFullReplay    : return "fullreplay";
        case ePartialReplay : return "partialreplay";
        case eBatch         : return "batch";
        default             : return "";
    }
}//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get workflow state as string
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfExecutive::State()
{
   switch(workflowState)
   {
        case eInitialize    : return "initialized";
        case ePause         : return "paused";
        case eRun           : return "running";
        case eStep          : return "stepping";
        case eStop          : return "stopped";
        case eWait          : return "waiting";
        default             : return "";
   }
}


void FbsfExecutive::waitCompletion(int aNbTasks)
{
    taskCond.acquire(aNbTasks);
}

void FbsfExecutive::resetWorking(int aNbSequences)
{
    iterCond.release(aNbSequences);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// number of working with iterations called
/// from Executive controller
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfExecutive::stillWorking()
{
    return iterCond.available();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Performance meter
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfExecutive::getPerfMeterInitial()
{
    // declare export
    mCpuStepTime =
        FbsfDataExchange::declarePublicData("Executive:CpuStepTime",
                                            cInteger,
                                            FbsfDataExchange::cExporter,
                                            "Executive","ms","Step time");
    // return initalization time as a string
    return  ";"+QString::number(mCpuInitializationTime);
}
QString FbsfExecutive::getPerfMeterFinal()
{
    return  ";"+QString::number(mCpuFinilizationTime);
}
QString FbsfExecutive::getPerfMeterStep()
{
    return  ";"
          + QString::number(mCpuStepTime->getIntValue()) + ";"
          + ";";
}
