#ifndef FBSFBASEMODEL_H
#define FBSFBASEMODEL_H
#include <QString>
#include <QList>
#include <QPair>
#include <QMap>
#include <QVector>
#include <QDataStream>
#include <QObject>
#include <QDebug>
#include <float.h>

#include "FbsfGlobal.h"
#include "ParamProperties.h"

class FbsfDataExchange;
typedef  float real;

// Dealing with the different types of data in ZE (used by extending classes)
// _enum: the enumerator
// _map:  the map between the Qstring and the Enumerator
// Modification merge from CEA version
enum class DataTypeZE: int
{
     typReal,
     typBool,
     typInt
};

static QMap<QString,DataTypeZE> DataTypeZE_map = {
    {QString("real"),DataTypeZE::typReal},
    {QString("bool"),DataTypeZE::typBool},
    {QString("int"),DataTypeZE::typInt}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_BASEMODEL_EXPORT FBSFBaseModel
                : public QObject
{
        Q_OBJECT
public:
    explicit FBSFBaseModel();
    virtual ~FBSFBaseModel();

    static FBSFBaseModel*  loadModule(QString   aModuleType,
                                      QString   aModuleName,
                                      float     aTimeStep,
                                      QObject*  rootWindow=0);
    static FBSFBaseModel*  loadFMUModel(QString modelName, QString aModelName,
                                        float   aStep,
                                        float   aStart=0.0,
                                        float   aEnd=FLT_MAX,
                                        bool    aDumpCsv=false);
    enum Transition
    {
        cPreSaveState,
        cPostSaveState,
        cPreRestoreState,
        cPostRestoreState
    };

    //~~~~~~~~~~~~~ Implementation of the workflow management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            initialize();
    void            finalize();
    void            consumeData();
    void            computeStep();

    virtual int     doInit()=0;         // Executed when thread's model start
    void            preStep();          // Executed before each step to import external values
    virtual int     doStep()=0;         // Execution of step computation (must be defined)
    virtual void    postStep();         // Executed after each step to export external values
    virtual int     doTerminate()=0;    // Executed when application exit
    virtual int     doSaveState();      // Executed when application request dump state to memory
    virtual int     doRestoreState();   // Executed when application request reload state from memory
    virtual int     doSaveState(QDataStream &out);     // dump state to file
    virtual int     doRestoreState(QDataStream& in);   // Restore state from file
    virtual int     doTransition(FBSFBaseModel::Transition aEvent){Q_UNUSED(aEvent);return 1;}
    virtual QMap<QString, ParamProperties> getParamList() {return {};}

    virtual bool        isNode(){return false;}
    // Performance meter
    QString             getPerfMeterInitial();
    QString             getPerfMeterFinal();
    QString             getPerfMeterStep();

    //~~~~~~~~~~~~~ Set of methods to link produced, consumed and state data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            publish(QString aName, int *aAddress,QString aUnit="", QString aDescription="",
                            bool aHistory=true);
    void            publish(QString aName,float *aAddress,QString aUnit="", QString aDescription="",
                            bool aHistory=true);
    void            publish(QString aName, QVector<int>* aVector,
                            QString aUnit="", QString aDescription="",
                            int aTimeShift=0,int aIndex=0,bool aHistory=true);
    void            publish(QString aName, QVector<real> *aVector,
                            QString aUnit="", QString aDescription="",
                            int aTimeShift=0,int aIndex=0,bool aHistory=true);
    void            publishAppType(FbsfAppType aType,QString aName,int *aAddress,
                              QString aUnit="", QString aDescription="",
                                bool aHistory=true);
    void            publishAppType(FbsfAppType aType,QString aName,float *aAddress,
                              QString aUnit="", QString aDescription="",
                                bool aHistory=true);

    void            publishValue(QString aName,int aValue);
    void            publishValue(QString aName, real aValue);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            subscribe(QString aName,int *aAddress,QString aUnit="", QString aDescription="");
    void            subscribe(QString aName,float *aAddress,QString aUnit="", QString aDescription="");
    void            subscribe(QString aName,QVector<int>* aVector,QString aUnit="", QString aDescription="");
    void            subscribe(QString aName,QVector<real> *aVector, QString aUnit="", QString aDescription="");
    void            subscribe(QString aName,QVariant& aData,QString aUnit="", QString aDescription="");
    void            subscribeAppType(FbsfAppType aType, QString aName, int *aAddress, QString aUnit="", QString aDescription="");
    void            subscribeAppType(FbsfAppType aType, QString aName, float *aAddress, QString aUnit="", QString aDescription="");

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            state(QString aName,int *aAddress);
    void            state(QString aName,real *aAddress);
    void            state(QString aName,QVector<int>* aVector);
    void            state(QString aName,QVector<real> *aVector);
    static void     consumeStateData(QDataStream& in){QVariantMap data;in>>data;}//consume stream
    //~~~~~~~~~~~~~ Accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int             status(){return mStatus;}
    void            name(QString aName) {mName = aName;}
    QString         name() {return mName;}
    void            simulationTime(float aSimTime) { mSimulationTime=aSimTime;}
    const float&    simulationTime() { return mSimulationTime;}
    void            timeStep(float aTimeStep) { mTimeStep=aTimeStep;}
    const float&    timeStep() { return mTimeStep;}
    //~~~~~~~~~~~~~~ Model configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QMap<QString,QString>& config(){return mConfig;}
    void            config(QMap<QString,QString> aConfig){mConfig=aConfig;}
    //~~~~~~~~~~~~~~ Application global configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QMap<QString,QString>& AppConfig(){return mAppConfig;}
    void            AppConfig(QMap<QString,QString>& aAppConfig){mAppConfig=aAppConfig;}
protected :
    //~~~~~~~~~~~~~~ for visual models ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            rootWindow(QObject* rootWindow){mRootWindow=rootWindow;}
    QObject*        rootWindow(){return mRootWindow;}
    int             loadVisualDocument(QString aDocument);
    int             initializeVisualDocument();
    int             computeVisualDocument();
    int             saveStateVisualDocument();
    int             restoreStateVisualDocument();
    int             saveStateVisualDocument(QDataStream& out);
    int             restoreStateVisualDocument(QDataStream& in);
    QObject*        getQmlPlugin(){return mQmlPlugin;}// accessor for visual documents
    virtual void    notifyParamChanged(QString aName, QVariant aValue);
signals :
    // enable executive control from modules (batch mode)
    void ExecutiveControl(QString,QString param=QString());
public slots :
    // enable notification control from modules or GUI
    void ParamChanged(QString aName, QVariant aValue){notifyParamChanged(aName,aValue);}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// members
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public :
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subclass for data saving/reloading (Polymorph type)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class StateData
    {
    public :
        StateData() = default;
        StateData(FbsfDataTypes aType,FbsfDataClass aClass,void* aAddress)
            : mDataType(aType)
            , mDataClass(aClass)
            , pData(aAddress)
        {}
        StateData (const StateData & Copy)
        {
            mDataType=Copy.mDataType;
            mDataClass=Copy.mDataClass;
            pData=Copy.pData;
            mData=Copy.mData;
        }
        ~StateData()  {}
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //  Convert pointed data to QVariant (used for saving)
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        QVariant    dataVariant()
        {
            if(mDataType==FbsfDataTypes::cInteger)
            {
                if(mDataClass==cScalar)
                    return QVariant::fromValue(*(int*)pData);
                else
                    return QVariant::fromValue(*(static_cast<QVector<int>*>(pData)));
            }
            else // cReal
            {
                 if(mDataClass==cScalar)
                     return QVariant::fromValue(*(real*)pData);
                 else
                    return QVariant::fromValue(*(static_cast<QVector<real>*>(pData)));
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        /// \brief StateData::operator = : to push back deserialized values
        /// \param Right : source data
        /// \return
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        StateData& operator=( const QVariant& Right )
        {
             // copy the reloaded value to local data (pointer)
            if(mDataType==cInteger)
            {
                if(mDataClass==cScalar)
                    *(static_cast<int*>(pData))= Right.toInt();
                else
                    *(static_cast<QVector<int>*>(pData))= Right.value<QVector<int>>();
            }
            else // cReal
            {
                if(mDataClass==cScalar)
                    *(static_cast<real*>(pData))= Right.toFloat();
                else
                   *(static_cast<QVector<real>*>(pData))= Right.value<QVector<real>>();
            }
            return *this;
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        static void registerTypes()
        {
            qRegisterMetaType<QVector<int>>("QVector<int>");
            qRegisterMetaTypeStreamOperators<QVector<int>>("QVector<int>");

            qRegisterMetaType<QVector<real>>("QVector<float>");
            qRegisterMetaTypeStreamOperators<QVector<real>>("QVector<float>");
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        friend QDebug operator<<(QDebug dbg, const FBSFBaseModel::StateData &data);

    private :
        FbsfDataTypes   mDataType;  // cInteger/cReal
        FbsfDataClass   mDataClass; // cScalar/cVector
        void*           pData;      // Polymorphic pointer to internal data
        QVariant        mData;
    };
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
protected :
    QVariantMap             mStateDataValueMap;// map of state data value
    QObject*                mQmlPlugin;     // handle for visual plugin (main.qml)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private :
    //~~~~~~~~~~~~~~~~~~~~~~~ Public data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QList<QPair<void*,FbsfDataExchange*>>   mProducedData;
    QList<QPair<void*,FbsfDataExchange*>>   mConsumedData;
    //~~~~~~~~~~~~~~~~~~~~~~~ State data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QMap<QString,StateData>                 mStateDataRefMap;  // map of state data reference
    //~~~~~~~~~~~~~~~~~~~~~~~ Privates members ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QString                 mName;          // Module instance name
    int                     mStatus;        //failed 0, success=1
    QMap<QString,QString>   mConfig;        // Module configuration
    QMap<QString,QString>   mAppConfig;     // Application configuration

    QObject*                mRootWindow;    // Visual root window

    float                   mSimulationTime; // seconds
    float                   mTimeStep;       // seconds

    bool                    mFirstStep;     // initial step indicator

    int                     mCpuInitializationTime=0;
    int                     mCpuFinilizationTime=0;
    int                     mCpuPreTime=0;
    int                     mCpuStepTime=0;
    int                     mCpuPostTime=0;
};
QDebug operator<<(QDebug dbg, const FBSFBaseModel::StateData &data);


#endif // FBSFBASEMODEL_H
