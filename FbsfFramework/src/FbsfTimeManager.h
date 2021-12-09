#ifndef FBSFTIMEMANAGER_H
#define FBSFTIMEMANAGER_H

#include <QObject>
#include <QVariant>
#include <QDateTime>

class FbsfDataExchange;
class FbsfTimeManager:public QObject
{
    Q_OBJECT
public:
    enum eTimeMode {eDateTime, eElapsedTime};
    enum eResolution {eSeconds=1000, eMilliSeconds=1};

    // Initial setup
    void setup(bool aMode, QMap<QString,QString>& aConfig, float aTimeStepS);
    void setDateTime(QString aDateStart, QString aTimeStart,
                     QString aDateFormat, QString aTimeFormat);
    void setModeSTD();

    void setModeMPC(int       aPastsize,
                    int       aFutursize,
                    int       aTimeshift);
    // Synch with cycle activity
    void    progress() ;


    qint64  dataDateTime();
    void    setDataDateTime(qint64 aDataDateTime);

    // MPC mode
    const QVector<int>& MPCDataDateTime() const;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///////////////////////// QML interface ////////////////////
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Q_INVOKABLE QString dateTimeStr(int aTime); // for date and time display
    Q_INVOKABLE QString dateStr(int aTime);     // for date display
    Q_INVOKABLE QString timeStr(int aTime);     // for time display
    Q_INVOKABLE int     historySize();
    Q_INVOKABLE int     historyWithFuturSize();

    // properties for QML
    Q_PROPERTY(QString  DateTimeStr  READ DateTimeStr NOTIFY DateTimeStrChanged)
    Q_PROPERTY(uint     StepCount    READ stepCount   NOTIFY stepCountChanged)
    Q_PROPERTY(bool     isUnitMS     READ isUnitMS CONSTANT)

    Q_PROPERTY(int      PastSize  READ pastsize CONSTANT)
    Q_PROPERTY(int      FuturSize READ futursize CONSTANT)
    Q_PROPERTY(int      TimeShift READ timeshift CONSTANT)

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// properties accessors
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QString DateTimeStr();             // current simulation time display
    uint    stepCount();
    void    setStepCount(uint aCount);
    bool    isUnitMS() {return mResolution==eMilliSeconds;}
    int     pastsize() const;
    int     futursize() const;
    int     timeshift() const;

private :
    // members accessors
    qint64 elapsedTimeMS();

    int    timeStepMS() const;
    void   setTimeStepMS(float aTimeStepS);

    QString ElapsedTimeMSStr(qint64 aElapsedMS);
    void    setDisplayFormat(QString aFormat);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
signals:
    void    DateTimeStrChanged();
    void    stepCountChanged();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private:
    bool        mModeMPC=false;
    eTimeMode   mTimeMode=eDateTime;
    eResolution mResolution=eSeconds;

    // initial date and time
    QDate       mDateStart;         // origin date user defined
    QTime       mTimeStart;         // origin time user defined
    QDateTime   mStartDateTime;     // current or user defined origin date-time

    // format for date and time
    QString     mDateFormat="dd/MM/yyyy";
    QString     mTimeFormat="HH:mm:ss";
    QString     mDateTimeFormat;    // default or user defined DateTime format

    // mode standard and MPC
    qint64      mDateTimeOrigin=0;  // origin timestamp

    qint64      mDataDateTime=0;     // computed date-time
    qint64      mElapsedTimeMS=0;    // computed elapsed time (millisecondes)

    int         mTimeStepMS=0;       // simulation step time (dt)
    uint        mStepCount=0;        // step count

    // mode MPC
    QVector<int> mMPCDataDateTime;   // published MPC time vector
    int         mPastsize=0;
    int         mFutursize=0;
    int         mTimeshift=1;

    // exchange public data
    FbsfDataExchange*   mPublicDataTime;
};

#endif // FBSFTIMEMANAGER_H
