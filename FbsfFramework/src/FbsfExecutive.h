#ifndef FBSFEXECUTIVE_H
#define FBSFEXECUTIVE_H

#include "FbsfGlobal.h"
#include "FbsfControler.h"
#include "FbsfApplication.h"

#include <QThread>
#include <QTime>
#include <QString>
#include <QSemaphore>
#include <QVariant>
#include <QList>
#include <QWaitCondition>

class FbsfSequence;
class FbsfNetClient;
class FbsfNetServer;
class FbsfDataExchange;

class FBSF_FRAMEWORKSHARED_EXPORT FbsfExecutive : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(FbsfExecutive)

// public methods
public:
    explicit        FbsfExecutive(eApplicationMode aMode);
    virtual         ~FbsfExecutive();
    int             addSequence(QString aName, float aPeriod,
                                QList<FbsfConfigNode>& aNodes,
                                QList<QMap<QString, QString> > &aModels,
                                FbsfApplication* app);
    QList<FbsfSequence*>& sequences(){return mSequenceList;}
    virtual void    setOptPerfMeter(bool aFlag){sOptPerfMeter=aFlag;}


    void            doCycle();                    // execute one cycle
    int             getStatus()             {return mStatus;};
    // configuration settings
    void            Period(uint aPeriod)     {mPeriod=mCycleTime=aPeriod;}
    uint            Period()                {return mPeriod;}
    void            Recorder(uint aSize)    {mRecorderSize=aSize;}
    uint            Recorder()              {return mRecorderSize;}
    void            ReplayMode(bool aFlag,QString aFile=QString());
    bool            ReplayMode()            {return mReplayMode;}
    void            BatchMode(bool aFlag);
    bool            BatchMode()            {return mExeMode==eBatch;}
    void            stepNumber(uint aNum)    {mStepNumber=aNum;}
    uint            stepNumber()            {return mStepNumber;}
    void            Speed(float aFactor)    {mSpeedFactor=aFactor;mCycleTime=mSpeedFactor*mPeriod;}
    void            fastSpeed(float aFast)  {mFastSpeedFactor=aFast;}
    QString         State();
    QString         ExecutionMode();
    QMap<QString,FBSFBaseModel*>& modelMap() {return mModelMap;}
    // Causes the current thread to sleep for msecs milliseconds.
    void            suspend(unsigned long msecs)
                    {
                        bSuspended=true;
                        QMutex mutex;
                        mutex.lock();
                        waitCondition.wait(&mutex, msecs);
                        mutex.unlock();
                        bSuspended=false;
                    }
    void stopApp() {
        emit exit();
    }
    // Causes the current thread to wakeup
    void            wakeup(){ waitCondition.wakeOne();bSuspended=false;}
    bool            isSuspended() { return bSuspended;}

    // Performance meter
    QString             getPerfMeterInitial();
    QString             getPerfMeterFinal();
    QString             getPerfMeterStep();
private :
    QWaitCondition  waitCondition;
    bool            bSuspended;

// signals
signals:
    void            cycleStart();               // signal start of major cycle
    void            cancelStep();
    void            consume();                  // signal data consumption
    void            compute();                  // signal step computation
    void            statusChanged(QVariant  mode,QVariant  state);// signal status change to UI
    void            exit();                     // exit application
// slots
public slots:
    void            run();                      // run the executive workflow (thread)
    void            control(QString command, QString param1=QString(),QString param2=QString());   // control of the simulation workflow
    void            errorString(QString err);   // error message collector
// members
private:
    QSemaphore   taskCond;
    QSemaphore   iterCond;

    void    waitCompletion(int aNbTasks);
    void    resetWorking(int aNbSequences);
    void    cancelWorking();
    int     stillWorking();

    enum state {eInitialize,ePause,eRun,eStep,eStop,eWait}workflowState;
    eApplicationMode        mAppMode;            // Application mode : standalone, server, client
    eExecutionMode          mExeMode;            // Execution mode : compute,replay or batch
    bool                    mReplayMode;         // mode is replay
    QString                 mReplayFile;         // file to replay
    int                     mReplayLength;       // Number of steps to replay
    float                   mCycleTime;          // period*speed factor (ms) for computation loop
    uint                    mPeriod;             // period (ms) for computation loop
    float                   mSimulationTime;     // Simulation time
    bool                    bComputeTime;        // Compute or not Simulation time
    uint                    mStepNumber;         // step number for a run
    float                   mSpeedFactor;        // Simulation speed (slow or accelerate)
    float                   mFastSpeedFactor;    // Simulation fast speed
    uint                    mRecorderSize;       // Recording limit size
    FbsfDataExchange*       mPublicSimulationTime;
    int                     mStatus;             // FBSF_OK, FBSF_ERROR
    uint                    mMultiSteps;         // Number of step to execute
    bool                    bExitAfterSteps;     // Action exit when steps done

    QList<FbsfSequence*>    mSequenceList;       // Sequence list
    QMap<QString,FBSFBaseModel*> mModelMap;      // model map container
    QSemaphore              mPauseCond;          // trigger pause
    FbsfControler           mDataControler;      // Data controler
    static FbsfNetClient*   mNetClient;          // Network Client
    static FbsfNetServer*   mNetServer;          // Network Server

    static bool             sOptPerfMeter;
    long long               mCpuInitializationTime=0;
    long long               mCpuFinilizationTime=0;
    FbsfDataExchange*       mCpuStepTime=nullptr;

};

#endif // FBSFEXECUTIVE_H
