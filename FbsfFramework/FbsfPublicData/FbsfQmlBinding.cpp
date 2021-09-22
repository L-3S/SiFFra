#include "FbsfQmlBinding.h"
#include "FbsfPublicData.h"
#ifndef MODE_BATCH

#include <QQmlProperty>
//#define TRACE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfQmlExchange
    \brief binding QML importer/exporter with public Data Exchange
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfQmlExchange::FbsfQmlExchange( QQuickItem *aParent )
        : QQuickItem    ( aParent )
        , pDataExchange (0)
        , m_tag1("")
        , m_tag2("")
        , m_data_type(cNoType)
        , m_unit("")
        , m_description("")
        , m_isBacktrack(false)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Get public data tag
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfQmlExchange::getExchangeName()
const
{
    if (m_tag2.isEmpty())
        return m_tag1;
    else
        return QString( "%1.%2" ).arg( m_tag1, m_tag2 );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set subtag (name of instance)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfQmlExchange::setTag1( const QString& atag )
{
    if( m_tag1 != atag ) m_tag1 = atag;

    // Later declaration for exported vectors from UI (see setPublicData)
    if (   m_class==cVector
        && m_flags==FbsfDataExchange::cExporter) return;
    declare(); // scalar and imported vectors
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Exchange declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfQmlExchange::declare(int aSize,int aTimeShift,int aTimeIndex)
{
    QString instance;
    // get the producer name
    if(parent()->objectName()=="NODE") // Declaration from FbsfEditor Document
    {
        //if (m_flags==FbsfDataExchange::cExporter ) // Bug 263
        {
            if(parent()->parent()==nullptr)
            {
                instance="NODE:"+getExchangeName();
            }
            else if (parent()->parent()->parent()->objectName()=="DOCUMENT")
            {
                // Exchange/Node/Graphic Image
                instance=parent()->parent()->parent()->parent()->objectName();
                instance+=":"+((QQuickItem*)parent()->parent())->property("pageId").toString();
                instance+=":"+((QQuickItem*)parent())->property("nodeId").toString();
            }
            else
            {
                // moduel Name
                instance=((QQuickItem*)parent()->parent()->parent()->parent())->property("nameModule").toString();

                if (((QQuickItem*)parent()->parent()->parent())->property("componentsType").toString()=="Graphic")
                    m_isBacktrack=true;
                #ifdef TRACE
                qDebug() << "QML Document Export" << getExchangeName() << (m_data_type == cInteger?"I":"R") << "from"
                         << instance
                         <<((QQuickItem*)parent()->parent())->property("pageId").toString()
                         <<((QQuickItem*)parent())->property("nodeName").toString();
                qDebug() << "\t" << "size:" << aSize << "timeshift:"<< aTimeShift<< "timeindex:"<<aTimeIndex;
                #endif
            }
        }
    }
    else
    {
        instance=parent()->objectName();
        m_isBacktrack=true;
        #ifdef TRACE
        qDebug() << "QML Graphic"
                 << (m_flags==FbsfDataExchange::cExporter?"Export":"Import")
                 << getExchangeName()
                 << (m_data_type == cInteger?"I":"R")
                 << "from" << instance;
        #endif
    }
    // Do not declare with an empty name
    if(getExchangeName()=="")
    {
        #ifdef TRACE
        qDebug() << "Empty exchange name for"
                 << instance
                 <<((QQuickItem*)parent()->parent())->property("pageId").toString()
                 <<((QQuickItem*)parent())->property("nodeId").toString();
        #endif
        return;
    }
    pDataExchange =
            FbsfDataExchange::declarePublicData(getExchangeName(),
                                                m_data_type,
                                                m_flags,
                                                instance,m_unit,m_description,
                                                m_class,aSize,
                                                aTimeShift,aTimeIndex);

    if (pDataExchange && m_flags==FbsfDataExchange::cImporter)
    {
        pDataExchange->m_qml_exchange.append(this);
        emit tag1Changed();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Set subtag (name of variable)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfQmlExchange::setTag2( const QString& atag )
{
    if( m_tag2 != atag )
    {
        m_tag2 = atag;
        emit tag2Changed();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfExporterIntItem
    \brief binding QML integer published public Data
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterIntItem::FbsfExporterIntItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
{
    m_data_type = cInteger;
    m_flags = FbsfDataExchange::cExporter;
    m_class = cScalar;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterIntItem::~FbsfExporterIntItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExporterIntItem::setIntValue( const int aValue )
{
    if(pDataExchange==nullptr)return;
    setValue(aValue);
    // propagate value from QML to QML
    if(pDataExchange && !pDataExchange->m_qml_exchange.isEmpty())
        for (int i=0;i<pDataExchange->m_qml_exchange.count();i++)
             pDataExchange->m_qml_exchange[i]->setIntValue(aValue);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfImporterIntItem
    \brief binding QML integer consummed public Data
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterIntItem::FbsfImporterIntItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
{
    m_data_type =cInteger;
    m_flags = FbsfDataExchange::cImporter;
    m_class = cScalar;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterIntItem::~FbsfImporterIntItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfImporterIntItem::setIntValue( const int aValue )
{
    emit valueChanged( aValue );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfExporterRealItem
    \brief binding QML real published public Data
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterRealItem::FbsfExporterRealItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
{
    m_data_type = cReal;
    m_flags = FbsfDataExchange::cExporter;
    m_class = cScalar;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterRealItem::~FbsfExporterRealItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExporterRealItem::setRealValue( const float aValue )
{
    if(pDataExchange==nullptr)
        return;
    setValue(aValue);

    // propagate value from QML to QML
    if(!pDataExchange->m_qml_exchange.isEmpty())
        for (int i=0;i<pDataExchange->m_qml_exchange.count();i++)
             pDataExchange->m_qml_exchange[i]->setRealValue(aValue);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfImporterRealItem
    \brief binding QML consummed public Data of type real
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterRealItem::FbsfImporterRealItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
{
    m_data_type = cReal;
    m_flags     = FbsfDataExchange::cImporter;
    m_class     = cScalar;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterRealItem::~FbsfImporterRealItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfImporterRealItem::setRealValue( const float aValue )
{
    emit valueChanged( aValue );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterVectorIntItem::FbsfImporterVectorIntItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
{
    m_data_type = cInteger;
    m_flags     = FbsfDataExchange::cImporter;
    m_class     = cVector;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterVectorIntItem::~FbsfImporterVectorIntItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfImporterVectorIntItem::setData(QList<int> aVector )
{
    emit dataChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterVectorRealItem::FbsfImporterVectorRealItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
{
    m_data_type = cReal;
    m_flags     = FbsfDataExchange::cImporter;
    m_class     = cVector;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfImporterVectorRealItem::~FbsfImporterVectorRealItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfImporterVectorRealItem::setData(QList<qreal> aList )
{
    emit dataChanged(); // emit onDataChanged signal to QML
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfExporterVectorIntItem
    \brief binding QML integer published public Data
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterVectorIntItem::FbsfExporterVectorIntItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
    , isDeclared(false)
    , mTimeShift(0)
    , mTimeIndex(0)
{
    m_data_type = cInteger;
    m_flags     = FbsfDataExchange::cExporter;
    m_class     = cVector;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterVectorIntItem::~FbsfExporterVectorIntItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExporterVectorIntItem::setPublicData(QList<int> aList)
{
    if (isDeclared)
    {
        FbsfQmlExchange::setPublicData(aList);// Copy vector to Public Data
        if(!pDataExchange->m_qml_exchange.isEmpty())
            for (int i=0;i<pDataExchange->m_qml_exchange.count();i++)
                 pDataExchange->m_qml_exchange[i]->setData(aList);
    }
    else
    {
        isDeclared=true;
        declare(aList.count(),mTimeShift,mTimeIndex);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfExporterVectorRealItem
    \brief binding QML integer published public Data
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterVectorRealItem::FbsfExporterVectorRealItem( QQuickItem *parent )
    : FbsfQmlExchange( parent )
    , isDeclared(false)
    , mTimeShift(0)
    , mTimeIndex(0)
{
    m_data_type = cReal;
    m_flags     = FbsfDataExchange::cExporter;
    m_class     = cVector;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfExporterVectorRealItem::~FbsfExporterVectorRealItem()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfExporterVectorRealItem::setPublicData(QList<qreal> aList)
{
    if (isDeclared)
    {
        FbsfQmlExchange::setPublicData(aList); // Copy vector to Public Data
        if(!pDataExchange->m_qml_exchange.isEmpty())
            for (int i=0;i<pDataExchange->m_qml_exchange.count();i++)
                 pDataExchange->m_qml_exchange[i]->setData(aList);
    }
    else
    {
        isDeclared=true;
        declare(aList.count(),mTimeShift,mTimeIndex);
    }
}
#endif
