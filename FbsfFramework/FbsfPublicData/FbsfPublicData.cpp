#include "FbsfPublicData.h"
#include "FbsfPublicDataModel.h"
#include "FbsfQmlBinding.h"
#include "FbsfPublicDataRecorder.h"
// static members initialization
static FbsfPublicDataTrace opttrace;
QMap<QString, FbsfDataExchange*> FbsfDataExchange::sPublicDataMap;
bool  FbsfDataExchange::simuMpc=true;
uint  FbsfDataExchange::m_recorderSize=0;
bool  FbsfDataExchange::m_replay_mode=false;
bool  FbsfDataExchange::m_replay_full=false;
int   FbsfDataExchange::m_replay_step=0;
#include <QDebug>
#include <QUrl>
#include <QDataStream>
#ifndef MODE_BATCH
#include <QQuickItem> // for QML_DECLARE_TYPE
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static function pointer to push export data via executive
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
STATICFUNC    FbsfDataExchange::executiveCB=nullptr;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfDataExchange
    \brief Public data pool management
    Public memory area for data exchange between models and MMI
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfDataExchange::FbsfDataExchange(QString      aTag,
                                   FbsfDataTypes aDataType,
                                   Flags        aFlags,
                                   QString      aProducer,
                                   QString      aUnit,
                                   QString      aDescription,
                                   FbsfDataClass aDataClass,
                                   int          aSize,
                                   int          aTimeShift,
                                   int          aTimeIndex,
                                   int          aHistory)
    : m_producer    ( aProducer)
    , m_tag         ( aTag )
    , m_data_type   ( aDataType )
    , m_flags       ( aFlags )
    , m_unit        ( aUnit )
    , m_description ( aDescription )
    , m_data_class  ( aDataClass )
    , m_size        ( aSize )
    , p_data        (nullptr)
    , p_recorder    (nullptr)
    , p_backtrack_data (nullptr)
    , m_replayed    (false)
    , m_qml_exchange (0)
{
    if (aDataClass == cScalar) {
        m_value.mInteger = 0;           // Scalar (Integer or Float)
        m_backtrack_value.mInteger = 0;
    } else if (aFlags==cExporter || aDataClass == cVector) {
        p_data = new FBSFValue[aSize];  // Vector
        p_backtrack_data = new FBSFValue[aSize];
    }
    // set recording if exported
    //if (aFlags==cExporter && aHistory) recorder(aTimeIndex,aTimeShift);
    if (aHistory) recorder(aTimeIndex,aTimeShift);

    // TODO : after initialization create a recorder if unresolved
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Destructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfDataExchange::~FbsfDataExchange()
{
    if (p_data) delete p_data;
    if (p_backtrack_data) delete p_backtrack_data;
    if (p_recorder) delete p_recorder;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Declaration of variable with tag name and public allocated address
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfDataExchange*
FbsfDataExchange::declarePublicData(QString     aName,
                                    FbsfDataTypes aType,
                                    Flags       aFlags,
                                    QString     aInstance,
                                    QString     aUnit,
                                    QString     aDescription,
                                    FbsfDataClass aDataClass,
                                    int         aSize,
                                    int         aTimeShift,
                                    int         aTimeIndex,
                                    int         aHistory)
{
    QString vName=aName.trimmed();
    FbsfDataExchange *publicAddress = FbsfDataExchange::sPublicDataMap.value(vName);

    if (publicAddress)
    {   // Exchange already declared
        // update its description in case of producer post declaration
        if (aFlags==cExporter)
        {
            if (publicAddress->m_flags!=cExporter)
            {
                publicAddress->producer(aInstance);
                publicAddress->flags(aFlags);
                publicAddress->unit(aUnit);
                publicAddress->description(aDescription);

                if (aDataClass==cVector)
                {
                    publicAddress->size(aSize); // producer size
                    #ifndef MODE_BATCH
                    FbsfdataModel::sFactoryListviewDataModel()->addData(publicAddress,true);//true to insert
                    #endif
                }
                if (aHistory) publicAddress->recorder(aTimeIndex,aTimeShift);

            }
            else if (! FbsfDataExchange::ReplayMode())  // ERROR multiple export case
            {
                QString msg("[WARNING] PublicData "
                            + vName +  " exported from " + aInstance
                            + " is already published from "
                            + publicAddress->producer());

                qDebug() << msg;
                return publicAddress;
            }
        }
        if (aType !=publicAddress->Type()) // ERROR mismatch type
        {
            QString msg("[WARNING] PublicData "
                        + vName  + " type mismatch "
                        + aType + " from " + aInstance
                        + " and " + publicAddress->Type()
                        + " from " + publicAddress->producer());

            qDebug() << msg;
            return publicAddress;
        }

        if(opttrace.trace())
            qDebug() << "[PublicData]"
                     << aInstance << "connect"
                     << (aFlags==cExporter?"Export":"Import")
                     << (aDataClass==cScalar?"Scalar":"Vector")
                     << "size:"<<(aDataClass==cScalar?1:aSize)
                     << vName
                     << ((aFlags==cExporter && aHistory)?"recorder ON":"")
                     << ((aFlags==cExporter && aTimeShift>0)?
                             QString("timeshift/index:%1/%2").arg(aTimeShift).arg(aTimeIndex):"")
                     << publicAddress;
    }
    else
    {   // first declaration
        QString producer;
        if (aFlags==cExporter) producer=aInstance;
        else aFlags = (FbsfDataExchange::Flags)(aFlags | cUnresolved);//Temporary set
        publicAddress = new FbsfDataExchange(vName,aType,aFlags,
                                             producer,aUnit,aDescription,
                                             aDataClass,aSize,
                                             aTimeShift,aTimeIndex,aHistory);
        if (opttrace.trace())
            qDebug() << "[PublicData]"
                     << aInstance << "declare"
                     << (aFlags==cExporter?"Export":"Import")
                     << (aDataClass==cScalar?"Scalar":"Vector")
                     << "size:"<<(aDataClass==cScalar?1:aSize)
                     << vName
                     << ((aFlags==cExporter && aHistory)?"recorder ON":"")
                     << ((aFlags==cExporter && aTimeShift>0)?
                             QString("timeshift/index:%1/%2").arg(aTimeShift).arg(aTimeIndex):"")
                     << publicAddress;
        // Insert produced data to link with the model
        sPublicDataMap.insert(vName,publicAddress );
        // Insert data reference for QML List
        #ifndef MODE_BATCH
        FbsfdataModel::sFactoryListviewDataModel()->addData(publicAddress);
        #endif
    }
#ifndef MODE_BATCH
    FbsfdataModel::sFactoryListviewDataModel()->addProducer(publicAddress->producer());
#endif
    // Fill the consumer list if import
    if ((aFlags & cImporter || aFlags & cUnresolved) && !aInstance.isEmpty())
    {
        if (!publicAddress->m_consumers.contains(aInstance)) // case one module inport several time the same variable
        {
            if(!publicAddress->m_consumers.isEmpty())
                publicAddress->m_consumers+=",";
            publicAddress->m_consumers += aInstance;
        }
    }
    return publicAddress;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get variable value as QVariant
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant FbsfDataExchange::value() const
{
    switch (Type())
    {
    case cReal    :return QVariant(m_value.mReal);
    case cInteger :return QVariant(m_value.mInteger);
    default : return 0;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get variable value of vector as QVariant
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant FbsfDataExchange::vectorValue(int aInd) const
{
    if (aInd >= m_size) qDebug() << aInd << "Out of bound index for" << name();
    if (p_data==nullptr) return QVariant();// case if not resolved
    switch (Type())
    {
    case cReal    :return QVariant(p_data[aInd].mReal); // mReal/mDouble according real
    case cInteger :return QVariant(p_data[aInd].mInteger);
    default : return 0;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// push QML value to public data pool (notify client Executive)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::value(int aValue)
{
    // m_value.mInteger = aValue;
    setIntValue(aValue);
    // push value in case of network client
    if(executiveCB!=nullptr)  notifyExport(name(),QVariant(aValue));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// push QML value to public data pool (notify client Executive)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::value(float aValue)
{
    //m_value.mReal = aValue;
    setRealValue(aValue);
    // push value in case of network client
    if(executiveCB!=nullptr) notifyExport(name(),QVariant(aValue));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// from QML Set value of vector item to data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::vectorValue(int aInd,int aValue)
{
    if (aInd >= m_size) qDebug() << aInd << "Out of bound index for" << name();

    switch (Type())
    {
    case cReal    : p_data[aInd].mReal=aValue ; break;// mReal/mDouble according real
    case cInteger : p_data[aInd].mInteger=aValue; break;
    default : return;
    }
    // push vector value in case of network client
    //TODO VECTOR : if(executiveCB!=NULL) notifyExport(name(),QVariant(aValue));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// from QML Set value of vector item to data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::vectorValue(int aInd,qreal aValue)
{
    if (aInd >= m_size) qDebug() << aInd << "Out of bound index for" << name();

    switch (Type())
    {
    case cReal    : p_data[aInd].mReal=(float)aValue ;break; // mReal/mDouble according real
    case cInteger : p_data[aInd].mInteger=aValue ;break;
    default : return;
    }
    // push vector value in case of network client
    //TODO VECTOR : if(executiveCB!=NULL) notifyExport(name(),QVariant(aValue));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Update and propagate value to QML if any UI connected
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::setIntValue ( const int aValue, bool backtrack )
{
    if (backtrack == true)
        m_backtrack_value.mInteger = aValue;
    else
        m_value.mInteger = aValue;
#ifndef MODE_BATCH
    if(!m_qml_exchange.isEmpty())
        for (int i=0;i<m_qml_exchange.count();i++)
            m_qml_exchange[i]->setIntValue(aValue);
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Update and propagate value to QML if any UI connected
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::setRealValue( const float aValue, bool backtrack )
{
    if (backtrack == true) {
        m_backtrack_value.mReal = aValue;
    } else {
        m_value.mReal = aValue;
    }
#ifndef MODE_BATCH
    if(!m_qml_exchange.isEmpty()) {
        for (int i=0;i<m_qml_exchange.count();i++) {
            m_qml_exchange[i]->setRealValue(aValue);
        }
    }
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Update and propagate value to QML if any UI connected
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::setData (QVector<int>* aVector, bool backtrack )
{
    if (backtrack == true) {
        memcpy(p_backtrack_data,aVector->data(),size()*sizeof(int));
    } else {
        memcpy(p_data,aVector->data(),size()*sizeof(int));
    }
#ifndef MODE_BATCH
    // Check if exist a QML consummer
    if(!m_qml_exchange.isEmpty())
        for (int i=0;i<m_qml_exchange.count();i++)
            m_qml_exchange[i]->setData((*aVector).toList());
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Update and propagate value to QML if any UI connected
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::setData (QVector<real>* aVector, bool backtrack )
{
    if (backtrack == true)
        memcpy(p_backtrack_data,aVector->data(),size()*sizeof(real));
    else
        memcpy(p_data,aVector->data(),size()*sizeof(real));

    // Check if exist a QML consummer
#ifndef MODE_BATCH
    if(!m_qml_exchange.isEmpty())
    {
        QList<qreal> vList;// copy due to QML (use Qist<double> only)
        for (int i=0; i < aVector->size();i++)   vList.append((*aVector)[i]);
        for (int i=0;i<m_qml_exchange.count();i++)
            m_qml_exchange[i]->setData(vList);
    }
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Update  vector values QVariantList (used on client side)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::setData (const QVariantList& aVectorValues, bool backtrack )
{
    // Update public data area
    for (int i = 0; i < aVectorValues.size(); i++)
    {
        if (Type()==cInteger) {
            if (backtrack == true)
                p_backtrack_data[i].mInteger = aVectorValues[i].toInt();
            else
                p_data[i].mInteger = aVectorValues[i].toInt();
        } else {
            if (backtrack == true)
                p_backtrack_data[i].mReal = aVectorValues[i].toInt();
            else
                p_data[i].mReal = aVectorValues[i].toFloat();
        }
    }
    // Check if exist a QML consummer
    if(!m_qml_exchange.isEmpty())
    {
        if (Type()==cInteger)
        {
            QList<int> vList;
            for (int i=0; i < aVectorValues.size();i++)
                vList.append(p_data[i].mInteger);
#ifndef MODE_BATCH
            for (int i=0;i<m_qml_exchange.count();i++)
                m_qml_exchange[i]->setData(vList);
#endif
        }
        else
        {
            // copy due to QML (use Qist<double> only)
            QList<qreal> vList;
            for (int i=0; i < aVectorValues.size();i++)
                vList.append(p_data[i].mReal);
#ifndef MODE_BATCH
            for (int i=0;i<m_qml_exchange.count();i++)
                m_qml_exchange[i]->setData(vList);
#endif
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// check if a variable is produced and published
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfDataExchange::isPublished( QString aName )
{
    if (sPublicDataMap.contains(aName))
    {
        FbsfDataExchange *publicAddress;
        publicAddress = FbsfDataExchange::sPublicDataMap.value(aName);
        return (publicAddress->FlagsAny(cExporter));
    }
    else
    {
        return false;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// setup or reset recorder object if historical data
/// aIndex : past/futur index (relevant for time depend vector)
/// aTimeShift : time steps shift (relevant for time depend vector)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::recorder(int aIndex,int aTimeShift)
{
    if(p_recorder!=nullptr) delete p_recorder;
    p_recorder=new FbsfPublicDataRecorder(this,aIndex,aTimeShift);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// record all the produced public data
/// aStep : current step number
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::processPublicData(int aStep)
{
    if(recorderSize()==0) return;
    QMutexLocker lock(&sProtectPublicData);

    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // record exported and unresolved public data
        if (data->FlagsAny(FbsfDataExchange::cExporter)
                || ( data->FlagsAny(FbsfDataExchange::cUnresolved) && (data->Class()==cScalar)))
        {
            data->record(aStep);
        }
    }
    m_replay_step=aStep;// keep current replay step
#ifndef MODE_BATCH
    emit FbsfdataModel::sFactoryListviewDataModel()->replayStepsChanged();
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// reset history of all the produced public data
/// aSimulationTime : time to reset to
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::resetHistory(float aTime)
{
    // get time index from "Simulation.Time" history
    FbsfDataExchange *publicAddress;
    publicAddress = FbsfDataExchange::sPublicDataMap.value("Simulation.Time");
    QVariantList& timeList=publicAddress->history();

    int stepNumber=timeList.indexOf(publicAddress->value());
    if (stepNumber==-1)
    {
        stepNumber=0;// reset full history
    }
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // get exported public data
        if (data->FlagsAny(FbsfDataExchange::cExporter)
                || ( data->FlagsAny(FbsfDataExchange::cUnresolved) && (data->Class()==cScalar)))
        {
            data->resetHistory(stepNumber);
        }
    }
    timeList=publicAddress->history();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Backtrack func
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::switchBacktrack(bool flag)
{
    QMap<QString, FbsfDataExchange*> publicData = FbsfDataExchange::sPublicDataMap;

    if (flag == true) {
        for (auto it = publicData.begin(); it != publicData.end(); it++) {
            if (it.value()->isUnresolved() || it.key() == "Simulation.Time")
                continue;
            it.value()->backtracked(true);
            if (simuMpc) {
                it.value()->backtrackHistory(it.value()->historywithfutur());
            } else {
                if (it.value()->Class() == cVector) {
                    QList<QVariant> tmp;
                    for (int i = 0; i < it.value()->size(); i += 1) {
                        tmp.push_back(it.value()->history(i));
                    }
                    it.value()->backtrackHistory(tmp);
                } else {
                    it.value()->backtrackHistory(it.value()->history());
                }
            }
        }
    } else {
        for (auto it = publicData.begin(); it != publicData.end(); it++) {
            if (it.value()->backtracked()) {
                it.value()->backtracked(false);
                it.value()->backtrackHistory(QVariantList());
            }
        }
    }
}

void FbsfDataExchange::updateBacktrack(int aStep)
{
    if(recorderSize()==0) return;
    QMutexLocker lock(&sProtectPublicData);

    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        if (data->backtracked() == true && data->FlagsAny(FbsfDataExchange::cExporter))
        {
            data->setBacktrack(aStep);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// record all the history data to file
/// aFileName : name of the file to write to
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::writeDataToFile(QString aFileName)
{
    //qDebug() << "Data recording to" << aFileName;
    QUrl url(aFileName);
    QFile file(url.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) qDebug() <<file.errorString() ;
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_7);
    out << static_cast<quint32>(FBSF_MAGIC);
    out << 1 ;// format number

    // get the replay length from recorded history of Simulation.Time
    FbsfDataExchange *publicAddress;
    publicAddress = FbsfDataExchange::sPublicDataMap.value("Simulation.Time");
    out << publicAddress->historySize();

    // process serialization
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // get name/value from exported public data
        if (data->FlagsAny(FbsfDataExchange::cExporter)
                || ( data->FlagsAny(FbsfDataExchange::cUnresolved) && (data->Class()==cScalar)))
        {
            data->serialize(out);
        }
    }
    file.close();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// load all the history data from file
/// aFileName : name of the file to read from
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfDataExchange::readDataFromFile(QString aFileName)
{
    QUrl url(aFileName);
    QFile file(aFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QString msg("Can't open replay file " + aFileName +" : " + file.errorString());
        qFatal(msg.toStdString().c_str());
    }
    QDataStream in(&file);

    //    if (in.version() !=QDataStream::Qt_5_7)
    //    {
    //        QString msg("Replay file " + aFileName +" : " +
    //                    "QT stream version Qt_5_7 mismatch");
    //        qFatal(msg.toStdString().c_str());
    //    }

    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != FBSF_MAGIC)
    {
        QString msg("Replay file " + aFileName +" : " +
                    "bad file format");
        qFatal(msg.toStdString().c_str());
    }
    // Read the format version
    qint32 format;
    in >> format;

    int replayDuration;
    in >> replayDuration; // get the replay number steps

    ReplayFull(true);// updated in deserialize()
    // deserialize from replay file
    while (!in.atEnd()) FbsfDataExchange::deserialize(in);
    // Close the replay file
    file.close();
    // Set the replay mode
    ReplayMode(true);

    return replayDuration;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Serialization of history to file.
/// aStream : data stream to write to
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::serialize(QDataStream& aStream)
{
    aStream << name()
            << Class()
            << timeShift()
            << timeIndex()
            << Type()
            << producer()
            << unit()
            << description()
            << size();

    if (p_recorder) return p_recorder->serialize(aStream);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Deserialization of data history from file
/// aStream : data stream to read from
/// Behaviour :
///     If the data is not produced, publish the replayed data
///     else reload the replayed data to produced the data history
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::deserialize(QDataStream& aStream)
{
    QString name,producer,unit,description;
    int Type,Class,timeShift,timeIndex;
    int size;
    aStream >> name
            >> Class
            >> timeShift
            >> timeIndex
            >> Type
            >> producer
            >> unit
            >> description
            >> size;

    if (!isPublished(name))
    {   // substitute declaration from replay data file if not published
        FbsfDataExchange* publicAddress =
                FbsfDataExchange::declarePublicData(name,
                                                    static_cast<FbsfDataTypes>(Type),
                                                    FbsfDataExchange::cExporter,
                                                    producer,unit,description,
                                                    static_cast<FbsfDataClass>(Class),
                                                    size,timeShift,timeIndex,true);
        publicAddress->p_recorder->deserialize(aStream);
        publicAddress->replayed(true);
    }
    else
    {
        //DEBUG << "Published" << name << "by" << producer;
        // Temporary creation, only used for deserialization
        FbsfDataExchange    publicAddress(name,
                                          static_cast<FbsfDataTypes>(Type),
                                          FbsfDataExchange::cExporter,
                                          producer,unit,description,
                                          static_cast<FbsfDataClass>(Class),
                                          size,timeShift,timeIndex,true);
        publicAddress.p_recorder->deserialize(aStream);
        FbsfDataExchange::ReplayFull(false); // At least one exporter
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Update data model
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfDataExchange::updateDataModel()
{
    #ifndef MODE_BATCH
    FbsfdataModel::sFactoryListviewDataModel()->updateValues();
    #endif
}
#ifndef MODE_BATCH
QML_DECLARE_TYPE(FbsfDataExchange)
#endif
