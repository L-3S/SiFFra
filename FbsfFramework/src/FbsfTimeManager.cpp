#include "FbsfTimeManager.h"
#include "FbsfPublicData.h"
#include <QDebug>
//#define TIMESTAMP
#define AUTO_RESOLUTION
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialize the time manager
// aMode        : mode STD or MPC
// aConfig      : xml config
// aTimeStepS   : time step as seconds[.ms]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setup(bool aMode,QMap<QString,QString>& aConfig,float aTimeStepS)
{
    mModeMPC=aMode;
    // set the time manager initial values
    QString dateStart = aConfig.value("startDate");
    QString timeStart = aConfig.value("startTime");
    QString dateFormat = aConfig.value("dateFormat");
    QString timeFormat = aConfig.value("timeFormat");
    // set TimeStep as ms
    setTimeStepMS(aTimeStepS);

    setDateTime(dateStart,timeStart,dateFormat,timeFormat);

    // set the specific behaviours
    if(mModeMPC)
    {
        int tmPastsize = aConfig.value("pastsize").toInt();
        int tmFutursize = aConfig.value("futursize").toInt();
        int tmTimeshift = aConfig.value("timeshift").toInt();
        setModeMPC(tmPastsize,tmFutursize,tmTimeshift);
    }
    else
    {
        setModeSTD();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// aDateStart : start date (default currentDate)
/// aTimeStart : start time (default currentTime)
/// DateFormat : format for date  (default "dd/MM/yyyy")
/// TimeFormat : format for time  (default "HH:mm:ss[.zzz]")
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setDateTime(QString aDateStart, QString aTimeStart,
                                  QString aDateFormat, QString aTimeFormat)
{
    // Check if cumulated time format is defined
    QRegExp rxh("^([h]{3})(:mm)?(:ss)?$");
    if(rxh.exactMatch(aTimeFormat))
    {
        mTimeMode=eCumulTime;
        mCumulUnit=eCumulHours;
        if(rxh.matchedLength()==6)
        {mCumulFormat=eCumulHM;} // hours:minutes
        else if(rxh.matchedLength()==9)
        {mCumulFormat=eCumulHMS;}// hours:minutes:seconds
    }
    QRegExp rxm("^([m]{3})(:ss)?$");
    if(rxm.exactMatch(aTimeFormat))
    {
        mTimeMode=eCumulTime;
        mCumulUnit=eCumulMinutes;
        if(rxm.matchedLength()==6)
            mCumulFormat=eCumulMS;//minutes:seconds
    }
    QRegExp rxs("^[s]{3}(.zzz)?$");
    if(rxs.exactMatch(aTimeFormat))
    {
        mTimeMode=eCumulTime;
        mCumulUnit=eCumulSeconds;
    }

#ifdef AUTO_RESOLUTION
    // set the milliseconde display according the timestep value
    if(mTimeStepMS%1000 != 0)
    {
        if(aTimeFormat.isEmpty())
        {   // set the milliseconde format
            mTimeFormat="HH:mm:ss.zzz";

            if(!aTimeStart.isEmpty())
            {
                // Check if ms set for time, if not set it to 000
                QRegExp rx("^(([0-1][0-9])|([2][0-3])):([0-5][0-9])(:([0-5][0-9]))([.]\\d{3})$");
                if(!rx.exactMatch(aTimeStart)) aTimeStart+=".000";
            }
        }
        else
        {
            // Check if ms set for time format, if not set it to .zzz
            QRegExp rx("^(.*)([.z][zz]?)$");
            if(!rx.exactMatch(aTimeFormat)) aTimeFormat+=".zzz";
        }
        mResolution=eResolution::eMilliSeconds;
    }
#endif
    // set the initial instant time or elapsedTime=0 as default
    if(mTimeMode==eCumulTime && !aTimeStart.isEmpty())
    {
        mTimeStart = QTime::fromString(aTimeStart,Qt::ISODate);
        mDateTimeOrigin=mTimeStart.msecsSinceStartOfDay();
        return;
    }

    // Check date/time set and date/time formats set
    if(!aDateFormat.isEmpty())mDateFormat=aDateFormat; // set the user format if defined

    if(!aTimeFormat.isEmpty())mTimeFormat=aTimeFormat; // set the user format if defined

    if(aDateStart.isEmpty() && aTimeStart.isEmpty())
    {
        // neither date and time defined
        if(aDateFormat.isEmpty() && aTimeFormat.isEmpty())
        {   // nor date and time format
            if(!mModeMPC)
            {
                // mode STD we work with an elapsed time
                mTimeMode=eElapsedTime;
            }
            else
            {
                // mode MPC set the start date and time to current
                // at first step
                setDisplayFormat(mDateFormat);
                setDisplayFormat(mTimeFormat);
            }
        }
        else
        {   // At least one format is defined
            if(!aDateFormat.isEmpty())
            {
                if(mModeMPC)
                {
                    //mDateStart=QDate::currentDate();
                    mStartDateTime.setDate(mDateStart);
                }
                setDisplayFormat(mDateFormat);
            }
            if(!aTimeFormat.isEmpty())
            {
                if(mModeMPC)
                {
                    // we must set the date to get an initial valid date-time
                    if(mDateStart.isNull())mStartDateTime.setDate(QDate::currentDate());
                    //mTimeStart=QTime::currentTime();
                    mStartDateTime.setTime(mTimeStart);
                }
                setDisplayFormat(mTimeFormat);
            }
        }
    }
    else
    {   // date or time defined, date format or time format defined
        // ISO 8601 extended format: either yyyy-MM-dd for dates
        // and HH:mm:ss[.zzz] for time (e.g. 2017-07-24T15:46:29[.000]),
        // Date part
        if(!aDateStart.isEmpty())
        {
            //  get the date as ISO 8601 format
            mDateStart = QDate::fromString(aDateStart,Qt::ISODate);
            if(!mDateStart.isValid())
            {
                QString msg="\tInvalid start date : "+aDateStart
                        + "\n\tCheck with ISO 8601 date format : yyyy-MM-dd";
                qFatal(msg.toStdString().c_str());
            }
            mStartDateTime.setDate(mDateStart);
            setDisplayFormat(mDateFormat);
        }
        else if(!aDateFormat.isEmpty())
        {
            // only format defined, set the current date with user defined format
            if(mModeMPC)
            {
                mDateStart=QDate::currentDate();
                mStartDateTime.setDate(mDateStart);
            }
            setDisplayFormat(mDateFormat);
        }
        // Time part
        if(!aTimeStart.isEmpty())
        {
            // set the start time as defined with user defined format or default
            mTimeStart = QTime::fromString(aTimeStart,Qt::ISODate);
            if(!mTimeStart.isValid())
            {
                QString msg="\tInvalid start time : "+aTimeStart
                        + "\n\tCheck with ISO 8601 time format : HH:mm:ss[.zzz]";
                qFatal(msg.toStdString().c_str());
            }
            // we must set the date to get an initial valid date-time
            if(mDateStart.isNull()) mStartDateTime.setDate(QDate::currentDate());

            mStartDateTime.setTime(mTimeStart);
            setDisplayFormat(mTimeFormat);
        }
        else if(!aTimeFormat.isEmpty())
        {
            // only time format defined, set the current
            // time with user defined format
            if(mModeMPC)
            {
                mTimeStart=QTime::currentTime();
                mStartDateTime.setTime(mTimeStart);
            }
            setDisplayFormat(mTimeFormat);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// set mode as Standard
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setModeSTD()
{
    qInfo() << "Time manager (mode STD) :";
    qInfo() << "\ttime step (ms) :" <<mTimeStepMS;
    // neither date/time nor formats are user defined
    if(mTimeMode==eElapsedTime)
    {
        qInfo() << "\ttime mode : elapsed time";
    }
    // Cumulated time is set
    else if(mTimeMode==eCumulTime)
    {
        QString unit;
        switch (mCumulUnit)
        {
        case eCumulHours   : unit="hours";  break;
        case eCumulMinutes : unit="minutes";break;
        case eCumulSeconds : unit="seconds";break;
        }
        qInfo() << "\ttime mode : cumulated time as"<<unit
                << "since" << mTimeStart.toString();
    }
    else
    {   // if start date or time unset we set current later at the first step
        if(mStartDateTime.isNull())
        {
            qInfo() << "\tStart date-time set at first step current time";
        }
        else
        {
            qInfo() << "\tStart date-time :"
                    << mStartDateTime.toString(mDateFormat + " " + mTimeFormat);

            if(mStartDateTime.isValid())
                mDateTimeOrigin=mResolution==eResolution::eMilliSeconds?
                            mStartDateTime.toMSecsSinceEpoch()
                          : mStartDateTime.toSecsSinceEpoch();
        }
    }

    // Set the current later at first step
#ifdef TIMESTAMP
    mDateTime=mDateTimeOrigin;
#endif
    emit DateTimeStrChanged();

    // Publish  Data.Time
    mPublicDataTime=
            FbsfDataExchange::declarePublicData("Data.Time",cInteger,
                                                FbsfDataExchange::cExporter,
                                                "FbsfTimeManager",isUnitMS()?"ms":"s",
                                                "Data Time");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// set mode MPC
/// aPastsize  : past vector size
/// aFutursize : future vector size
/// aTimeshift : step time shift
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setModeMPC(int     aPastsize,
                                 int     aFutursize,
                                 int     aTimeshift)
{
    qInfo() << "Time manager (mode MPC) :";
    qInfo() << "\ttime step (ms) :" <<mTimeStepMS;
    qInfo() << "\tpast time  :" <<aPastsize;
    qInfo() << "\tfutur time :" <<aFutursize;
    qInfo() << "\ttimeshift  :" <<aTimeshift;
    if(mTimeMode==eCumulTime)
    {
        QString unit;
        switch (mCumulUnit)
        {
        case eCumulHours : unit="hours";break;
        case eCumulMinutes : unit="minutes";break;
        case eCumulSeconds : unit="seconds";break;
        }
        qInfo() << "\ttime mode : cumulated time as"<<unit
                << "since" << mTimeStart.toString();

        // set initial instant time for display
        mDataDateTime=mTimeStart.msecsSinceStartOfDay()/mResolution;
    }
    else
    {// if start date or time unset we set current later at the first step
        if(mStartDateTime.isNull())
        {
            qInfo() << "\tStart date-time set at first step current time";
        }
        else
        {
            qInfo() << "\tStart date-time :"
            << mStartDateTime.toString(mDateFormat + " " + mTimeFormat);

            mDateTimeOrigin=mResolution==eResolution::eMilliSeconds?
                        mStartDateTime.toMSecsSinceEpoch()
                      : mStartDateTime.toSecsSinceEpoch();
        }
    }
    // set MPC parameters
    mPastsize=aPastsize;
    mFutursize=aFutursize;
    mTimeshift=aTimeshift;

    // set the vector size : past + futur
    mMPCDataDateTime.resize(mPastsize+mFutursize);
#ifdef TIMESTAMP
    mDateTime=mDateTimeOrigin;
#endif
    // set the present date/time for initial display
    mMPCDataDateTime[mPastsize-1]=mDataDateTime;
    emit DateTimeStrChanged();

    //    // Publish Simulation.Time and Data.Time
    mPublicDataTime=
            FbsfDataExchange::declarePublicData("Data.Time",cInteger,
                                                FbsfDataExchange::cExporter,
                                                "FbsfTimeManager",isUnitMS()?"ms":"s",
                                                "Data Time",
                                                cVector,mMPCDataDateTime.size(),
                                                aTimeshift,aPastsize);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// set the date/time as new line formatted
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setDisplayFormat(QString aFormat)
{
    if(mDateTimeFormat.isNull()) mDateTimeFormat=aFormat;
    else mDateTimeFormat +="\n"+aFormat;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// case STD mode check if date or time format is user defined
/// but date or time undefined
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::progress()
{
    static int firstStep=true;
    if(firstStep)
    {   // STD mode and at least one format defined
        if(!mDateTimeFormat.isEmpty())
        {
            // set the current date time of the first step if undefined
            if (mDateStart.isNull())
            {
                mDateStart=QDate::currentDate();
                mStartDateTime.setDate(mDateStart);
            }
            if (mTimeStart.isNull())
            {
                mTimeStart=QTime::currentTime();
                mStartDateTime.setTime(mTimeStart);
            }
        }
        if(mTimeMode!=eCumulTime)
        {
            // set the origin timestamp according resolution
            mDateTimeOrigin=mResolution==eResolution::eMilliSeconds?
                        mStartDateTime.toMSecsSinceEpoch()
                      : mStartDateTime.toSecsSinceEpoch();
        }
#ifdef TIMESTAMP
        mDataDateTime=mDateTimeOrigin;
#endif
        firstStep=false;
    }
    mElapsedTimeMS+=mTimeStepMS;

    if(mModeMPC)
    {   // MPC mode : Compute the data date and time
        if(mTimeMode==eCumulTime)
        {
            mDataDateTime = (mDateTimeOrigin+mElapsedTimeMS* mTimeshift)/mResolution;
        }
        else
        {
            mDataDateTime =(mElapsedTimeMS* mTimeshift)/mResolution ;
        }
        // Fill the published time vector
        QVector<int> pData(mPublicDataTime->size(),0);
        for (int i=0;i < mMPCDataDateTime.size();i++)
        {
            // last element in the Past will correspond to chosen Date
            mMPCDataDateTime[i]= (mDataDateTime + (i - (mPastsize-1))*mTimeStepMS/mResolution);
            // set time according resolution (s or ms) in ZE
            pData[i]=mMPCDataDateTime[i];
        }
        // Publish data date and time
        mPublicDataTime->setData(&pData);
    }
    else //~~~~~~~~~~~~ standard mode ~~~~~~~~~~~~~~
    {
#ifdef TIMESTAMP
        mDataDateTime += mDateTimeOrigin;
#else
        if(mTimeMode==eCumulTime)
            mDataDateTime = (mDateTimeOrigin+mElapsedTimeMS)/mResolution;
        else
            mDataDateTime = mElapsedTimeMS/mResolution;
#endif
        // Publish data date and time
        mPublicDataTime->setIntValue(mDataDateTime);
    }
    mStepCount++;
    emit DateTimeStrChanged();
    emit stepCountChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Member accessors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfTimeManager::timeStepMS() const
{
    return mTimeStepMS;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setTimeStepMS(float  aTimeStepS)
{
    mTimeStepMS = aTimeStepS*1000;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfTimeManager::stepCount()
{
    return mStepCount;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setStepCount(int aCount)
{
    mStepCount=aCount-1;
    mElapsedTimeMS=mStepCount*mTimeStepMS;
    progress();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
qint64 FbsfTimeManager::dataTimeStamp()
{
    if(mTimeMode==eElapsedTime)
        return (isUnitMS()?mElapsedTimeMS:mElapsedTimeMS/1000);
    qint64 timestamp=mDataDateTime;
    if(mTimeMode!=eCumulTime) timestamp+=dateTimeOrigin();// Datation
    return timestamp;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
qint64 FbsfTimeManager::elapsedTimeMS()
{
    return mElapsedTimeMS;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get history vector size
// STD historySize("Simulation.Time")-1
// MPC historySize("Simulation.Time")-1 + parseInt(futurSize)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfTimeManager::historySize()
{
    return mPublicDataTime->historySize();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get history vector size + futur size
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfTimeManager::historyWithFuturSize()
{
    return historySize()+mFutursize;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// MPC settings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfTimeManager::timeshift() const
{
    return mTimeshift;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfTimeManager::futursize() const
{
    return mFutursize;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfTimeManager::pastsize() const
{
    return mPastsize;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// reference to MPC time vector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const QVector<int>& FbsfTimeManager::MPCDataDateTime() const
{
    return mMPCDataDateTime;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// MPC Time vector access by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
qint64 FbsfTimeManager::dataTimeStamp(int index )
{
    if(index>mMPCDataDateTime.length()){
        qDebug()<<__FUNCTION__
                << "Index out of bound"
                << index << " > " << mPastsize+mFutursize;
        return 0;// ERROR
    }
    qint64 timestamp=mMPCDataDateTime[index];
    if(mTimeMode!=eCumulTime) timestamp+=dateTimeOrigin();
    return timestamp;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// display elapsed time with format : [d] hh:mm:ss[.ms]
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfTimeManager::ElapsedTimeMSStr(qint64 aElapsedMS)
{
    qint64 mseconds = aElapsedMS % 1000;
    qint64 seconds  = (aElapsedMS /1000) % 60;
    qint64 minutes  = (aElapsedMS / 60000) % 60;
    qint64 hours    = (aElapsedMS / 3600000) % 24;
    qint64 days     = (aElapsedMS / 86400000);

    QString timeString = days==0?"":QString::number(days);
    if(mResolution==eResolution::eMilliSeconds)
        timeString+= QString(" %1:%2:%3.%4")
                .arg(hours  , 2, 10, QChar('0'))
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'))
                .arg(mseconds,3, 10, QChar('0'));
    else
        timeString+= QString(" %1:%2:%3")
                .arg(hours  , 2, 10, QChar('0'))
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'));

    return timeString;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// display cumulated time with format : hhh mmm sss
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfTimeManager::CumulTimeMSStr(qint64 aElapsedMS)
{
    QString timeString;

    switch(mCumulUnit)
    {
    case eCumulHours:
    {
        qint64 seconds  = (aElapsedMS /1000) % 60;
        qint64 minutes  = (aElapsedMS / 60000) % 60;
        qint64 hours    = aElapsedMS / 3600000;
        switch (mCumulFormat)
        {
        case eCumulSingle : timeString=QString("%1 h").arg(hours);break;
        case eCumulHMS : timeString=QString("%1 h %2 mn %3 s").arg(hours)
                    .arg(minutes,2,10,QChar('0'))
                    .arg(seconds,2,10,QChar('0'));break;
        case eCumulHM : timeString=QString("%1 h %2 mn").arg(hours)
                    .arg(minutes,2,10,QChar('0'));break;
        default : break;
        }
        break;
    }
    case eCumulMinutes:
    {
        qint64 seconds  = (aElapsedMS /1000) % 60;
        qint64 minutes  = aElapsedMS / 60000;
        switch (mCumulFormat)
        {
        case eCumulSingle : timeString=QString("%1 mn").arg(minutes);break;
        case eCumulMS : timeString=QString("%1 mn %2 s").arg(minutes)
                    .arg(seconds,2,10,QChar('0'));break;
        default : break;
        }
        break;
    }
    case eCumulSeconds:
    {
        qint64 mseconds = aElapsedMS % 1000;
        qint64 seconds  = aElapsedMS /1000;
        if(mResolution==eResolution::eMilliSeconds)
            timeString=QString("%1.%2").arg(seconds).arg(mseconds,3,10,QChar('0'));
        else
            timeString=QString("%1 s").arg(seconds);
        break;
    }
    }

    return timeString;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// return display formatted string for current simulation date/time
///      elapsed        : d hh:mm:ss:ms
///      cumul          : hhh mmm sss
///      date time      : dd/MM/yyyy hh:mm:ss
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// test local format
//  QLocale locale(QLocale("en_US"));
//  return locale.toString(DateTime,mDateTimeFormat);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfTimeManager::DateTimeStr()
{
    if(mTimeMode==eElapsedTime)
    {   // display elapsed time
        return ElapsedTimeMSStr(mElapsedTimeMS);
    }
    else if(mTimeMode==eCumulTime)
    {   // display cumulated time
        if(mModeMPC)
        {
            qint64 dataTime=mMPCDataDateTime[mPastsize-1]*mResolution;
            return CumulTimeMSStr(dataTime);
        }
        else
        {
            return CumulTimeMSStr(mDateTimeOrigin+mElapsedTimeMS);
        }
    }
    else
    {   // display date and/or time according format
        qint64 dataDateTime;
        if(mModeMPC)
            dataDateTime=mMPCDataDateTime[mPastsize-1];
        else
            dataDateTime=mDataDateTime;

        if(!mStartDateTime.isValid()) return mDateTimeFormat;

#ifndef TIMESTAMP
        dataDateTime+=mDateTimeOrigin;
#endif

        QDateTime DateTime=mResolution==eResolution::eMilliSeconds?
                    QDateTime::fromMSecsSinceEpoch(dataDateTime)
                  :QDateTime::fromSecsSinceEpoch(dataDateTime);

        return DateTime.toString(mDateTimeFormat);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// QML call with aTime as seconds or millisecondes
/// Plotters, Table view , ....
/// aTime : elapsed time s or ms from start
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfTimeManager::dateTimeStr(int aTime)
{//qDebug() <<__FUNCTION__<< aTime;
    if(mTimeMode==eElapsedTime)
    {
        return ElapsedTimeMSStr(aTime*mResolution);
    }
    else if(mTimeMode==eCumulTime)
    {
        // display elapsed time
        return CumulTimeMSStr(aTime*mResolution);
    }
    else
    {
        qint64 dataDateTime=aTime;

#ifndef TIMESTAMP
        dataDateTime+=mDateTimeOrigin;
#endif
        QDateTime DateTime=mResolution==eResolution::eMilliSeconds?
                    QDateTime::fromMSecsSinceEpoch(dataDateTime)
                  :QDateTime::fromSecsSinceEpoch(dataDateTime);

        return DateTime.toString(mDateTimeFormat);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfTimeManager::dateStr(int aTime)
{
    QStringList dateTime=dateTimeStr(aTime).split("\n");
    return dateTime.length()>0?dateTimeStr(aTime).split("\n")[0]:"";
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QString FbsfTimeManager::timeStr(int aTime)
    {
    QStringList dateTime=dateTimeStr(aTime).split("\n");
    return dateTime.length()>1?dateTimeStr(aTime).split("\n")[1]:"";
    }
