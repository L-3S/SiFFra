#include "FbsfReplayFileAPI.h"
#include "FbsfPublicData.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///  @file    FbsfReplayFileAPI.cpp
///  @author  G. ROUAULT
///  @date    09/06/2018
///  @version FBSF V1.9
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/** @example main.cpp
 * A description of the example file, causes the example file to show up in
 * Examples */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Open the replay file and get all data
/// @param[in] aFile the data file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfReplayFileAPI::FbsfReplayFileAPI(QString aFile)
{
    mReplayDuration = FbsfDataExchange::readDataFromFile(aFile);
    FbsfDataExchange::ReplaySteps(mReplayDuration);
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qDebug()<< "FBSF Data file reader API";
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qDebug() << "Data file :" << aFile;
    qDebug() << "Data mode :" << (isStandardMode()?"Standard":"Time depend");
    qDebug() << "Duration :" << mReplayDuration << "steps";
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfReplayFileAPI::~FbsfReplayFileAPI()
{
    qDebug()<< "Replay file reader API Terminate ";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief get the data names list
/// @param[out] aList reference to the caller QList<QString>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfReplayFileAPI::getDataList(QList<QString>& aList)
{
    QMap<QString, FbsfDataExchange*>::iterator i;
     for (i = FbsfDataExchange::sPublicDataMap.begin();
          i != FbsfDataExchange::sPublicDataMap.end(); ++i)
         aList << i.key() ;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief check if the mode is Standard or Time depend
/// @return  true or false
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfReplayFileAPI::isStandardMode()
{
    return ! FbsfDataExchange::sPublicDataMap.contains("Data.Time");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Constructor
/// @param aName the name of the data to lookup
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DataInfo::DataInfo(QString aName)
{
    mPublicAddress = FbsfDataExchange::sPublicDataMap.value(aName);
    if(mPublicAddress==NULL)
        qDebug()<< "[Warning] Data" << aName << "not found";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data producer name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString  DataInfo::Producer() const
{
    if(mPublicAddress==NULL) return QString();
    return mPublicAddress->producer();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data name
/// @return data name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString  DataInfo::Name() const
{
    if(mPublicAddress==NULL) return QString("invalid data");
    return mPublicAddress->name();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data class (DataInfo::cScalar or DataInfo::cVector)
/// @return data class code
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DataInfo::DataClass DataInfo::Class() const
{
    if(mPublicAddress==NULL) return (DataInfo::DataClass)0;
    return (DataInfo::DataClass)mPublicAddress->Class();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data class as string ("Scalar" or "Vector")
/// @return data class name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString  DataInfo::ClassString() const
{
    if(mPublicAddress==NULL) return QString();
    return mPublicAddress->ClassToStr();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data type (DataInfo::cInteger or DataInfo::cReal)
/// @return data type code
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DataInfo::DataTypes  DataInfo::Type() const
{
    if(mPublicAddress==NULL) return cNoType;
    return (DataInfo::DataTypes)mPublicAddress->Type();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data type as string ("Integer" or "Real")
/// @return data type name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString  DataInfo::TypeString() const
{
    if(mPublicAddress==NULL) return QString();
    return mPublicAddress->TypeToStr();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data description
/// @return data description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString  DataInfo::Description() const
{
    if(mPublicAddress==NULL) return QString();
    return mPublicAddress->description();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for data unit
/// @return data unit
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString DataInfo::Unit() const
{
    if(mPublicAddress==NULL) return QString();
    return mPublicAddress->unit();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for vector data size
/// @return vector size
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int DataInfo::Size() const
{
    if(mPublicAddress==NULL) return 0;
    return mPublicAddress->size();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for time depend shift value
/// @return data shift value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int DataInfo::TimeShift() const
{
    if(mPublicAddress==NULL) return 0;
    return mPublicAddress->timeShift();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for time depend index value (past/futur)
/// @return data past/futur index value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int DataInfo::TimeIndex() const
{
    if(mPublicAddress==NULL) return 0;
    return mPublicAddress->timeIndex();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for scalar data values
/// @return  QVariantList reference with values
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const QVariantList& DataInfo::Values() const
{
    if(mPublicAddress==NULL) return QVariantList();
    return mPublicAddress->history();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for vector data values at index
/// @param[in] aIndex : index for row
/// @return  QVariantList reference with values
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const QVariantList& DataInfo::VectorValues(int aIndex) const
{
    if(mPublicAddress==NULL) return QVariantList();

    if (aIndex> mPublicAddress->size())
    {
        qDebug()<< "[Warning] Data" << Name() << "bad Index"
                << aIndex << " is greater than size"
                << mPublicAddress->size() ;
        return QVariantList();
    }
    return mPublicAddress->history(aIndex);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @brief Getter for Time depend data values at given step
/// @param[in] aStep  step number
/// @return  QVariantList reference with values
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const QVariantList& DataInfo::TimeDependValues(int aStep) const
{
    if(mPublicAddress==NULL) return QVariantList();
    mPublicAddress->record(aStep);
    return mPublicAddress->historywithfutur();
}
