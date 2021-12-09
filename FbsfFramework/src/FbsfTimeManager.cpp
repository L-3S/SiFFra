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
    QString dateStart = aConfig.value("Date.start");
    QString timeStart = aConfig.value("Time.start");
    QString dateFormat = aConfig.value("Date.format");
    QString timeFormat = aConfig.value("Time.format");
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
    QRegExp rxs("^[s][s][s](.*)?$");
    if(rxs.exactMatch(aTimeFormat)) qDebug() << __FUNCTION__<< "cumul secondes";
    QRegExp rxm("^[m][m][m](.*)?$");
    if(rxm.exactMatch(aTimeFormat)) qDebug() << __FUNCTION__<< "cumul minutes";

    //    QString a1("sss");
    //    QString a2("sss.zzz");
    //    QString a3("ss");
    //    QString a4("ss.zzz");
    //    if(rx.exactMatch(a1)) qDebug() << __FUNCTION__<< "1 cumul secondes";
    //    if(rx.exactMatch(a2)) qDebug() << __FUNCTION__<< "2 cumul secondes";
    //    if(rx.exactMatch(a3)) qDebug() << __FUNCTION__<< "3 cumul secondes";
    //    if(rx.exactMatch(a4)) qDebug() << __FUNCTION__<< "4 cumul secondes";

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
                mDateStart=QDate::currentDate();
                mStartDateTime.setDate(mDateStart);
                mTimeStart=QTime::currentTime();
                mStartDateTime.setTime(mTimeStart);
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
                    mDateStart=QDate::currentDate();
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
                    mTimeStart=QTime::currentTime();
                    mStartDateTime.setTime(mTimeStart);
                }
                setDisplayFormat(mTimeFormat);
            }
        }
    }
    else
    {   // date or time defined, date format or time format defined
        // Date part
        if(!aDateStart.isEmpty())
        {
            // set the start date as defined with user defined format or default
            mDateStart = QDate::fromString(aDateStart,mDateFormat);
            if(!mDateStart.isValid())
            {
                QString msg="\tInvalid start date :"+aDateStart
                        + "\n\tCheck with expected date format : "+ mDateFormat;
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
            mTimeStart = QTime::fromString(aTimeStart,mTimeFormat);
            if(!mTimeStart.isValid())
            {
                QString msg="\tInvalid start time :"+aTimeStart
                        + "\n\tCheck with expected time format : "+ mTimeFormat;
                qFatal(msg.toStdString().c_str());
            }
            // we must set the date to get an initial valid date-time
            if(mDateStart.isNull())mStartDateTime.setDate(QDate::currentDate());

            mStartDateTime.setTime(mTimeStart);
            setDisplayFormat(mTimeFormat);
        }
        else if(!aTimeFormat.isEmpty())
        {
            // only format defined, set the current time with user defined format
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
    qInfo() << "\tStart date-time :"
            << mStartDateTime.toString(mDateFormat + " " + mTimeFormat);

    mDateTimeOrigin=mResolution==eResolution::eMilliSeconds?
                mStartDateTime.toMSecsSinceEpoch()
              : mStartDateTime.toSecsSinceEpoch();

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
/// set the dat/time as new line formatted
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
        if(!mModeMPC && !mDateTimeFormat.isEmpty())
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
        // set the origin timestamp according resolution
        mDateTimeOrigin=mResolution==eResolution::eMilliSeconds?
                    mStartDateTime.toMSecsSinceEpoch()
                  : mStartDateTime.toSecsSinceEpoch();
#ifdef TIMESTAMP
        mDateTime=mDateTimeOrigin;
#endif
        firstStep=false;
    }
    mElapsedTimeMS+=mTimeStepMS;

    if(mModeMPC)
    {   // Fill the published time vector
        mDataDateTime +=((mTimeStepMS/mResolution) * mTimeshift);
        QVector<int> pData(mPublicDataTime->size(),0);
        for (int i=0;i < mMPCDataDateTime.size();i++)
        {
            #ifdef MPC_ONLINE
            // first element in the Past will correspond to chosen Date
            mTimeData[i]= mDateTime + (i*mTimeStepMS/mResolution);
            #else
            // last element in the Past will correspond to chosen Date
            mMPCDataDateTime[i]= (mDataDateTime + (i - (mPastsize-1))*mTimeStepMS/mResolution);
            #endif
            // set time in seconds in ZE
            pData[i]=mMPCDataDateTime[i];
        }
        mPublicDataTime->setData(&pData);
    }
    else // standard mode
    {
#ifdef TIMESTAMP
        mDateTime = mDateTimeOrigin+mElapsedTimeMS/mResolution;
#else
        mDataDateTime += mTimeStepMS/mResolution;
#endif
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
uint FbsfTimeManager::stepCount()
{
    return mStepCount;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setStepCount(uint aCount)
{
    mStepCount=aCount;
    emit stepCountChanged();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
qint64 FbsfTimeManager::dataDateTime()
{
    return mDataDateTime;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfTimeManager::setDataDateTime(qint64 aDataDateTime)
{
    mDataDateTime = aDataDateTime;
    if(mModeMPC)
    {
        // Don't know what to do, see FbsfDataExchange::resetHistory() comment
    }
    else
    {
        mPublicDataTime->setIntValue(mDataDateTime);
    }
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
/// return display formatted string for current simulation date/time
///      elapsed       :  d hh:mm:ss:ms
///      utc date time : dd.MM.yyyy hh:mm:ss
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
