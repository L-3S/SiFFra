#ifndef FBSF_PUBLIC_DATA_H
#define FBSF_PUBLIC_DATA_H

#include <QtCore/qglobal.h>     // QT_VERSION, Q_DECL_EXPORT, Q_DECL_IMPORT

#if defined FBSF_PUBLIC_DATA
#define FBSF_INTERFACE Q_DECL_EXPORT
#else
#define FBSF_INTERFACE Q_DECL_IMPORT
#endif

#include <QString>
#include <QMutex>

#include <QAbstractListModel>
#include <QVariant>
#include <QVariantList>

#include "FbsfGlobal.h"
#include "FbsfPublicDataRecorder.h"
// The name of the time manager variable in exchange zone
//static const char* dataDateTime="Simulation.Time";
// TODO TM better to get it from FbsfTimeManager
static const char* dataDateTime="Data.Time";

typedef void ( *STATICFUNC ) ( QString, QVariant );
typedef float real;

static QVariantList sEmptyList; // error case
static QMutex       sProtectPublicData;  // mutex for history set and get

class FbsfQmlExchange;
class FbsfPublicDataRecorder;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Public Data trace option
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfPublicDataTrace
{
public :
    FbsfPublicDataTrace(): mTrace(false)
    {
        QString options=qgetenv("FBSF_TRACE");
        if (options.contains("PublicData", Qt::CaseInsensitive)) mTrace=true ;
    }
    bool trace(){return mTrace;}
private:
    bool mTrace;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Public Data exchange interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_INTERFACE FbsfDataExchange
{
public:
    enum Flags
    {
        cImporter   = 0x00000001,
        cExporter   = 0x00000002,
        cUnresolved = 0x00000004
    };
    // Exchange data
    typedef union
    {
        int         mInteger;
        float       mReal;
    } FBSFValue;
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FbsfDataExchange(QString                    aTag,
                     FbsfDataTypes              aDataType,
                     FbsfDataExchange::Flags    aFlags,
                     QString                    aProducer="undefined",
                     QString                    aUnit="",
                     QString                    aDescription="",
                     FbsfDataClass              aDataClass=cScalar,int size=1,
                     int aTimeShift=0, int aTimeIndex=0,int aHistory=true);

    virtual ~FbsfDataExchange();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static FbsfDataExchange*
    declarePublicData(QString       aName,
                      FbsfDataTypes aDataType,
                      FbsfDataExchange::Flags aFlags,
                      QString       aInstance="undefined",
                      QString       aUnit="",
                      QString       aDescription="",
                      FbsfDataClass aDataClass=cScalar,
                      int           aSize=1,
                      int           aTimeShift=0, int aTimeIndex=0,
                      int           aHistory=true);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // begin QML binding
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QString     name() const        { return m_tag;        }
    QString     producer() const    { return m_producer;   }
    QString     unit() const        { return m_unit;       }
    QString     description() const { return m_description;}


    void        producer(QString aProducer){ m_producer=aProducer;}
    void        flags(FbsfDataExchange::Flags aFlags){ m_flags=aFlags;}
    void        unit(QString aUnit)        { m_unit=aUnit;}
    void        description(QString aDescr){ m_description=aDescr;}

    QVariant    value() const;
    QVariant    vectorValue(int aInd) const;

    void        value(int aValue);
    void        value(float aValue);
    void        vectorValue(int aInd,int aValue);
    void        vectorValue(int aInd,qreal aValue);

    bool        isUnresolved()  {return FlagsAny(cUnresolved);}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // end QML binding
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    float     getRealValue( ) const { return m_value.mReal; }
    virtual void    setRealValue( const float aValue, bool backtrack = false );

    int       getIntValue ( ) const {return m_value.mInteger; }
    virtual void    setIntValue ( const int aValue, bool backtrack = false);

    real*      getRealAddress() {return &m_value.mReal; }

    virtual void    setData (QVector<int>* aVector, bool backtrack = false );
    virtual void    setData (QVector<real>* aVector, bool backtrack = false );
    virtual void    setData (const QVariantList &aVectorValues, bool backtrack = false );

    //~~~~~~~~~~~~ Type accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int             Type ( ) const          { return m_data_type; }
    QString         TypeToStr ( ) const
    { return m_data_type == cInteger? "Integer" : "Real" ; }
    static FbsfDataTypes StrToType (const QString aType )
    { return (aType == "Integer"? cInteger:cReal); }

    //~~~~~~~~~~~~ Class accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int             Class ( ) const          { return m_data_class; }
    QString         ClassToStr ( ) const
    { return m_data_class == cScalar? "Scalar" : "Vector" ; }
    static FbsfDataClass StrToClass (const QString aClass )
    { return (aClass == "Scalar"? cScalar:cVector); }
    //~~~~~~~~~~~~ application type accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            appType (FbsfAppType aType ){ m_app_type=aType; }
    int             appType ( ) const          { return m_app_type; }
    QString         appTypeToStr ( ) const
    { return m_app_type == cParameter? "Parameter" : "Constant" ; }
    static FbsfAppType StrToappType (const QString aAppType )
    { return (aAppType == "Parameter"? cParameter:cConstant); }
    //~~~~~~~~~~~~  accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    bool            FlagsAny    ( const int aFlags )const
    { return ( ( m_flags & aFlags ) != 0 ); }

    int             size()                  {return m_size;}
    void            size(int aSize)         {p_data = new FBSFValue[aSize];
                                             p_backtrack_data = new FBSFValue[aSize];
                                             m_size=aSize;}

    static bool     isPublished( QString aName );
    //~~~~~~~~~~~~~~ recorder accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            recorder(int aIndex,int aTimeShift);
    void            record(int aStep)       {if (p_recorder) p_recorder->record(aStep);}
    //~~~~~~~~~~~~~~ replay accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            replayed(bool aFlag)    {m_replayed=aFlag;}
    bool            replayed()              {return m_replayed;}
    //~~~~~~~~~~~~~~ backtrack accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            backtracked(bool aFlag)    {m_backtracked=aFlag;}
    bool            backtracked()              {return m_backtracked;}
    void            backtrackHistory(QVariantList aList)    {m_backtrackHistory=aList;}
    QVariantList    backtrackHistory()                      {return m_backtrackHistory;}
    void            setBacktrack(int aStep)       {if (p_recorder) p_recorder->setBacktrack(aStep);}
    //~~~~~~~~~~~~~~ serialization accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            serialize(QDataStream& aStream);
    static void     deserialize(QDataStream& aStream);
    //~~~~~~~~~~~~~~ historization accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int             historySize()   {QMutexLocker lock(&sProtectPublicData);
                                     if (p_recorder){return p_recorder->historySize();}else return 0;}
    QVariantList&   history()       {QMutexLocker lock(&sProtectPublicData);
                                     if (p_recorder)return p_recorder->history();else return sEmptyList;}
    QVariantList&   historywithfutur(){QMutexLocker lock(&sProtectPublicData);
                                       if (p_recorder){return p_recorder->historywithfutur();}
                                       else{ return sEmptyList;}}
    QVariantList&   history(int aIndex) {QMutexLocker lock(&sProtectPublicData);
                                         if (p_recorder){return p_recorder->history(aIndex);}
                                         else{ return sEmptyList;}}
    void            resetHistory(int aStep){if (p_recorder){return p_recorder->resetHistory(aStep);}}
    //~~~~~~~~~~~~~~ Time depend accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int             timeShift()    { if (p_recorder) return p_recorder->timeShift(); else return 0;}
    int             timeIndex()    { if (p_recorder) return p_recorder->timeIndex(); else return 0;}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QString         consumers()     {return m_consumers;}

    static void            updateDataModel();
    // members
public :
    static QMap<QString, FbsfDataExchange*> sPublicDataMap;

    QString             m_producer;         // producer name
    QString             m_tag;              // public name
    FbsfDataTypes       m_data_type;        // Real, Integer
    Flags               m_flags;            // export, import
    FBSFValue           m_value;            // data value
    QString             m_unit;             // data display unit
    QString             m_description;      // data description
    FbsfDataClass       m_data_class;       // scalar , vector
    int                 m_size;             // data size
    FBSFValue*          p_data;             // data vector
    FbsfPublicDataRecorder*  p_recorder;     // Recorder manager

    bool                m_replayed;         // data is replayed

    bool                m_backtracked = false;      // data is backtracked
    QVariantList        m_backtrackHistory;   // data backtrack history
    FBSFValue           m_backtrack_value;
    FBSFValue*          p_backtrack_data;

    QVector<FbsfQmlExchange*> m_qml_exchange;// qml data binding

    QString             m_consumers;
    FbsfAppType         m_app_type;

    //    static QMutex       sProtectDeclaration;  // mutex for declaration

    // Backtrack mode implementation
    static bool simuMpc;
    static void switchBacktrack(bool flag);
    static void updateBacktrack(int aStep);

    // History mode implementation
    static uint m_recorderSize;
    static void recorderSize(uint aSize) {m_recorderSize=aSize;}    // Optional limit
    static uint recorderSize()           {return m_recorderSize;}   // get limit
    static void resetHistory();                                     // reset history stack

    // Broadcast public data
    static void processPublicData(int aStep);

    // Replay mode implementation
    static bool m_replay_mode;
    static bool m_replay_full;
    static int  m_replay_step;
    static void ReplayMode(bool flag)   {m_replay_mode=flag;}
    static bool ReplayMode()            {return m_replay_mode;}
    static void ReplayFull(bool flag)   {m_replay_full=flag;}
    static bool ReplayFull()            {return m_replay_full;}
    static int  ReplayStep()            {return m_replay_step;}
    static void ReplaySteps(int aNbSteps) {m_replay_step=aNbSteps;} // for API

    static void writeDataToFile(QString aFileName);     // recording for replay
    static int  readDataFromFile(QString aFileName);    // replay previous run

    // notify Client Executive for data export
    static  STATICFUNC    executiveCB;// to avoid QObject for this class
    void    notifyExport(QString aTag, QVariant aValue){ executiveCB(aTag,aValue);}
};


#endif // FBSF_PUBLIC_DATA_H
