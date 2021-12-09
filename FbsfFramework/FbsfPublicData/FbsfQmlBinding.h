#ifndef FBSFQMLBINDING_H
#define FBSFQMLBINDING_H
#include <QtCore/qglobal.h>     // QT_VERSION, Q_DECL_EXPORT, Q_DECL_IMPORT

#if defined FBSF_PUBLIC_DATA
#define FBSF_INTERFACE Q_DECL_EXPORT
#else
#define FBSF_INTERFACE Q_DECL_IMPORT
#endif
#include "FbsfPublicData.h"

#ifndef MODE_BATCH
#include <QQuickItem>
#include <QString>
#include <QDebug>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_INTERFACE FbsfQmlExchange
        : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfQmlExchange )
    Q_PROPERTY( QString tag1 READ getTag1 WRITE setTag1 NOTIFY tag1Changed )
    Q_PROPERTY( QString tag2 READ getTag2 WRITE setTag2 NOTIFY tag2Changed )

signals:
    //void            valueToBeExported( const QString& aExportName, QVariant aValue );

    void            tag1Changed ( );
    void            tag2Changed ( );

public:
    explicit    FbsfQmlExchange( QQuickItem *aParent = 0 );

    void            declare(int aSize=0, int aTimeShift=0, int aTimeIndex=0);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const QString&  getTag1   ( ) const               { return m_tag1; }
    void            setTag1   ( const QString& aTag );

    const QString&  getTag2   ( ) const               { return m_tag2; }
    void            setTag2   ( const QString& aTag );

    QString         getExchangeName( ) const;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // push exported scalar value to public data pool
    void            setValue(const int aValue)   {pDataExchange->value(aValue);}
    void            setValue(const float aValue) {pDataExchange->value(aValue);}
    // push exported vector data to public data pool
    void            setPublicData( const QList<int> aList)
    {for(int i=0;i < aList.count() ; i++) pDataExchange->vectorValue(i,aList[i]);}
    void            setPublicData( const QList<qreal> aList)
    {for(int i=0;i < aList.count() ; i++) pDataExchange->vectorValue(i,aList[i]);}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // QML binding
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // scalar getters
    int       getIntValue ( ) const
    {
        if (pDataExchange->backtracked() && m_isBacktrack) {
            return pDataExchange->m_backtrack_value.mInteger;
        } else {
            return pDataExchange->m_value.mInteger;
        }
    }
    float     getRealValue( ) const
    {
        if (pDataExchange->backtracked() && m_isBacktrack) {
            return pDataExchange->m_backtrack_value.mReal;
        } else {
            return pDataExchange->m_value.mReal;
        }
    }
    // Vector getters
    const QList<int>    getIntData ( ) const
    {   QList<int> data;
        if (m_isBacktrack && pDataExchange->backtracked() && pDataExchange!=nullptr && pDataExchange->p_backtrack_data) {
            for(int i=0;i < pDataExchange->size() ; i++)
                data.append( pDataExchange->p_backtrack_data[i].mInteger);
        } else if (pDataExchange!=nullptr && pDataExchange->p_data) {
            for(int i=0;i < pDataExchange->size() ; i++)
                data.append( pDataExchange->p_data[i].mInteger);
        }
        return data;
    }
    const QList<qreal>  getRealData( ) const
    {   QList<qreal> data;
        if (m_isBacktrack && pDataExchange->backtracked() && pDataExchange!=nullptr && pDataExchange->p_backtrack_data) {
            for(int i=0;i < pDataExchange->size() ; i++)
                data.append( pDataExchange->p_backtrack_data[i].mReal);
        } else if (pDataExchange!=nullptr && pDataExchange->p_data) {
            for(int i=0;i < pDataExchange->size() ; i++)
                data.append( pDataExchange->p_data[i].mReal);
        }
    return data;
}
    // history getter
    virtual QVariantList    getHistory() { return pDataExchange->history();}
    virtual QVariantList    getHistoryWithFutur() { return pDataExchange->historywithfutur();}
    // Unit and description
    virtual QString         getUnit() {return pDataExchange->unit();}
    virtual void            setUnit(QString aUnit) {m_unit=aUnit;}
    virtual void            setDescription(QString aDesc) {m_description=aDesc;}
    // TimeShift and TimeIndex
    virtual int             getTimeShift() {return pDataExchange==nullptr?0:pDataExchange->timeShift();}
    virtual int             getTimeIndex() {return pDataExchange==nullptr?0:pDataExchange->timeIndex();}

    // push scalar value to QML - redefined in subclass
    virtual void            setRealValue( const float aValue ){Q_UNUSED(aValue)}
    virtual void            setIntValue ( const int aValue ){Q_UNUSED(aValue)}
    // push vector data to QML - redefined in subclass
    virtual void            setData ( QList<int> aVector ){Q_UNUSED(aVector)}
    virtual void            setData ( QList<qreal> aVector ){Q_UNUSED(aVector)}
    virtual void            notifyChanged(){}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // members
    FbsfDataExchange*           pDataExchange;
    QString                     m_tag1;
    QString                     m_tag2;
    FbsfDataTypes               m_data_type;
    FbsfDataExchange::Flags     m_flags;
    FbsfDataClass               m_class;
    QString                     m_unit;
    QString                     m_description;
    bool                        m_isBacktrack;
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfExporterIntItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfExporterIntItem )
    Q_PROPERTY( int value READ getIntValue WRITE setIntValue)
    Q_PROPERTY( QString unit WRITE setUnit)
    Q_PROPERTY( QString description WRITE setDescription)
public:
    explicit FbsfExporterIntItem( QQuickItem *parent = nullptr );
    virtual ~FbsfExporterIntItem( ) override;
    virtual void    setIntValue     ( const int aValue ) override;
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfImporterIntItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfImporterIntItem )
    Q_PROPERTY( int value READ getIntValue WRITE setIntValue NOTIFY valueChanged )
    Q_PROPERTY( QVariantList history READ getHistory NOTIFY historyChanged)

    Q_PROPERTY( QString unit READ getUnit)

signals:
    void        valueChanged    ( const int aValue );
    void        historyChanged    ( );

public:
    explicit FbsfImporterIntItem( QQuickItem *parent = nullptr );
    virtual ~FbsfImporterIntItem( ) override;
    virtual void    setIntValue     ( const int aValue ) override;
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfExporterRealItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfExporterRealItem )
    Q_PROPERTY( float value READ getRealValue WRITE setRealValue )
    Q_PROPERTY( QString unit WRITE setUnit)
    Q_PROPERTY( QString description WRITE setDescription)

public:
    explicit FbsfExporterRealItem( QQuickItem *parent = nullptr );
    virtual ~FbsfExporterRealItem( ) override;
    virtual void    setRealValue    ( const float aValue ) override;
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfImporterRealItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfImporterRealItem )
    Q_PROPERTY( float value READ getRealValue WRITE setRealValue NOTIFY valueChanged )
    Q_PROPERTY( QVariantList history READ getHistory NOTIFY historyChanged)

    Q_PROPERTY( QString unit READ getUnit)

signals:
    void            valueChanged    ( const float aValue );
    void            historyChanged    ( );

public:
    explicit FbsfImporterRealItem( QQuickItem *parent = nullptr );
    virtual ~FbsfImporterRealItem( )override;
    virtual void    setRealValue    ( const float aValue )override;
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfImporterVectorIntItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfImporterVectorIntItem )
    Q_PROPERTY( QList<int> data READ getIntData WRITE setData NOTIFY dataChanged )
    Q_PROPERTY( QVariantList history READ getHistory )
    Q_PROPERTY( QVariantList historywithfutur READ getHistoryWithFutur )

    Q_PROPERTY( QString unit READ getUnit)

    Q_PROPERTY( int timeshift READ getTimeShift )
    Q_PROPERTY( int timeindex READ getTimeIndex )

signals:
    void            dataChanged    ();

public:
    explicit FbsfImporterVectorIntItem( QQuickItem *parent = nullptr );
    virtual ~FbsfImporterVectorIntItem( )override;
    virtual void    setData    (QList<int> aVector )override;
    virtual void    notifyChanged() override {emit dataChanged();}
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfImporterVectorRealItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfImporterVectorRealItem )
    Q_PROPERTY( QList<qreal> data READ getRealData WRITE setData NOTIFY dataChanged )
    Q_PROPERTY( QVariantList history READ getHistory )
    Q_PROPERTY( QVariantList historywithfutur READ getHistoryWithFutur )

    Q_PROPERTY( QString unit READ getUnit)

    Q_PROPERTY( int timeshift READ getTimeShift )
    Q_PROPERTY( int timeindex READ getTimeIndex )
signals:
    void            dataChanged    ();

public:
    explicit FbsfImporterVectorRealItem( QQuickItem *parent = nullptr );
    virtual ~FbsfImporterVectorRealItem( )override;
    virtual void    setData    (QList<qreal> aList )override;
    virtual void    notifyChanged() override {emit dataChanged();}
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfExporterVectorIntItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfExporterVectorIntItem )
    Q_PROPERTY( QList<int> data READ getIntData WRITE setPublicData NOTIFY dataChanged )
    Q_PROPERTY( int timeshift READ getTimeShift WRITE setTimeShift NOTIFY timeshiftChanged)
    Q_PROPERTY( int timeindex READ getTimeIndex WRITE setTimeIndex NOTIFY timeindexChanged)
    Q_PROPERTY( QString unit WRITE setUnit)
    Q_PROPERTY( QString description WRITE setDescription)
signals:
    void dataChanged();
    void timeshiftChanged();
    void timeindexChanged();
public:
    explicit FbsfExporterVectorIntItem( QQuickItem *parent = nullptr );
    virtual ~FbsfExporterVectorIntItem( )override;
    void setPublicData(QList<int> aList);
    void setTimeShift(int value){mTimeShift=value;}
    void setTimeIndex(int value){mTimeIndex=value;}
private :
    bool isDeclared;
    int mTimeShift;
    int mTimeIndex;
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfExporterVectorRealItem
        : public FbsfQmlExchange
{
    Q_OBJECT
    Q_DISABLE_COPY( FbsfExporterVectorRealItem )
    Q_PROPERTY( QList<qreal> data READ getRealData WRITE setPublicData NOTIFY dataChanged )
    Q_PROPERTY( int timeshift READ getTimeShift WRITE setTimeShift NOTIFY timeshiftChanged)
    Q_PROPERTY( int timeindex READ getTimeIndex WRITE setTimeIndex NOTIFY timeindexChanged)
    Q_PROPERTY( QString unit WRITE setUnit)
    Q_PROPERTY( QString description WRITE setDescription)
signals:
    void dataChanged();
    void timeshiftChanged();
    void timeindexChanged();
public:
    explicit FbsfExporterVectorRealItem( QQuickItem *parent = nullptr );
    virtual ~FbsfExporterVectorRealItem( )override;
    void setPublicData(QList<qreal> aList);
    void setTimeShift(int value){mTimeShift=value;}
    void setTimeIndex(int value){mTimeIndex=value;}
private :
    bool isDeclared;
    int mTimeShift;
    int mTimeIndex;
} ;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QML_DECLARE_TYPE( FbsfExporterIntItem )
QML_DECLARE_TYPE( FbsfImporterIntItem )
QML_DECLARE_TYPE( FbsfExporterRealItem )
QML_DECLARE_TYPE( FbsfImporterRealItem )

QML_DECLARE_TYPE( FbsfImporterVectorIntItem )
QML_DECLARE_TYPE( FbsfImporterVectorRealItem )
QML_DECLARE_TYPE( FbsfExporterVectorIntItem )
QML_DECLARE_TYPE( FbsfExporterVectorRealItem )
#endif

#endif // FBSFQMLBINDING_H
