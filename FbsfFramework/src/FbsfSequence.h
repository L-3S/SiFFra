#ifndef FBSFSEQUENCE_H
#define FBSFSEQUENCE_H
#include "FbsfGlobal.h"
#include "FbsfConfiguration.h"

#include <QList>
#include <QString>
#include <QObject>
#include <QSemaphore>

class FbsfApplication;
class FBSFBaseModel;
class FbsfDataExchange;
class FBSF_FRAMEWORKSHARED_EXPORT FbsfSequence
        : public QObject
{
    Q_OBJECT

public:
    virtual ~FbsfSequence();
    FbsfSequence(QString aName,float aPeriod,FbsfApplication * app, QSemaphore   &ptaskCond, QSemaphore   &piterCond);
    int             addModels(QList<QMap<QString, QString> > &aModels, QList<FbsfConfigNode> & aNodes);
    void            addModel(FBSFBaseModel* model);

    void            finalize();

    int             doSaveState(QDataStream&    out);  // Executed when application request dump state
    int             doRestoreState(QDataStream& in);   // Executed when application request reload state
    // Accessors
    fbsfStatus      status(){return mStatus;}
    void            name(QString aName) {mName = aName;}
    QString         name() {return mName;}
    QList<FBSFBaseModel*> getModelList() {return mModelList;}

    // Synchronization
    void                waitCompletion(int aNbTasks);
    void                resetWorking(int aNbSequences);
    int                 stillWorking();

    // Performance meter
    QString             getPerfMeterInitial();
    QString             getPerfMeterFinal();
    QString             getPerfMeterStep();

private:
    long long           mCpuInitializationTime=0;
    long long           mCpuFinilizationTime=0;
    FbsfDataExchange*   mCpuPreTime=nullptr;
    FbsfDataExchange*   mCpuStepTime=nullptr;
    FbsfDataExchange*   mCpuPostTime=nullptr;

private :
    QString             mName;
    float               mPeriod;        // Timestep ratio : >1 slower <1 faster
    int                 mIter;          // fast ratio
    int                 mRemainIter;
    int                 mStepNumber;
    FbsfApplication *   mApp;
    QList<FBSFBaseModel*>   mModelList;// List of registred models
    fbsfStatus          mStatus;       // failed 0, success=1
    QThread*            mThread;
    QSemaphore&         taskCond;
    QSemaphore&         iterCond;

private :
    void            signalCompletion();

public slots:
    void            initialize();
    void            cycleStart();
    void            cancelSeqStep();
    void            consumeData();
    void            computeStep();

signals:
    void            finished();
    void            cancelModelStep();
    void            error(QString err);
};

#endif // FBSFSEQUENCE_H
