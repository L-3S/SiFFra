#ifndef FBSFPULICDATAMODEL_H
#define FBSFPULICDATAMODEL_H
#include "FbsfPublicData.h"
#include <QDebug>
#ifndef MODE_BATCH
#include <QColor>
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//                 QML
//                  ||
//         FbsfFilterProxyModel
//                  ||
//              FbsfdataModel ===> FbsfDataModelItem{n} -> FbsfDataExchange{n}
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <QtCore/qglobal.h>     // QT_VERSION, Q_DECL_EXPORT, Q_DECL_IMPORT
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QDate>

#if defined FBSF_PUBLIC_DATA
    #define FBSF_INTERFACE Q_DECL_EXPORT
#else
    #define FBSF_INTERFACE Q_DECL_IMPORT
#endif

#ifndef MODE_BATCH
class FbsfDataExchange;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class for QML list item description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class FbsfDataModelItem
{
public:
    FbsfDataModelItem(FbsfDataExchange* aPublicData,FbsfDataModelItem* aParent=nullptr,int aIndex=-1)
        : mPublicData(aPublicData)
        , mIndex(aIndex)
        , mExpanded(false)
        , mChecked(false)
        , mColor(QColor())
        , mParent(aParent)
    {}
    int         index()                     {return mIndex;}
    QString     name()const                 {return mIndex==-1?
                                                     mPublicData->name()
                                                    : QString("%1[%2]").arg(mPublicData->name()).arg(mIndex);}
    QString     producer()const             {return mPublicData->producer();}
    QString     description()const          {return mPublicData->description();}
    QString     unit()const                 {return mPublicData->unit();}

    int         Type()                      {return mPublicData->Type();}
    int         Class()                     {return mPublicData->Class();}
    int         Size()                      {return mPublicData->size();}

    QVariant    value()const                {return mIndex==-1?
                                                    mPublicData->value()
                                                    :mPublicData->vectorValue(mIndex);}
    QVariant    itemValue(int aInd)const    {return mPublicData->vectorValue(aInd);}

    void        value(int aValue)           {mPublicData->value(aValue);}
    void        value(float aValue)         {mPublicData->value(aValue);}

    bool        isHeadVector()              {return (mPublicData->Class()==cVector && mIndex==-1);}
    bool        isHeadDataVector()          {return (isHeadVector() && timeShift() == 0);}
    bool        isHeadTimeDependVector()    {return (isHeadVector() && timeShift() > 0);}
    bool        isScalar()                  {return (mPublicData->Class()==cScalar);}
    bool        isParameter()               {return (mPublicData->appType()==cParameter);}
    bool        isConstant()                {return (mPublicData->appType()==cConstant);}
    bool        isItemVector()              {return mIndex>=0 && mPublicData->timeShift()==0;}
    void        toggleVisible()             {mExpanded=mExpanded==false?true:false;}
    bool        isVectorExpanded()          {return mExpanded;}
    bool        isItemVisible()             {return mIndex==-1?true:mParent?mParent->mExpanded:false;}
    bool        isUnresolved()              {return mPublicData->isUnresolved(); }

    void        checked(bool value)         { mChecked=value;}
    bool        checked()                   { return mChecked;}
    void        color(QColor value)         { mColor=value;}
    QColor      color()                     { return mColor;}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // accessor for history data management
    QVariantList&   history()               {return mIndex==-1?
                                                mPublicData->history() // scalar or time depend vector
                                               :mPublicData->history(mIndex); // data vector
                                            }
    QVariantList&   historywithfutur()      {return mIndex==-1?
                                                mPublicData->historywithfutur() // scalar or time depend vector
                                               :sEmptyList; // not relevant
                                            }
    int             timeShift()             {return mPublicData->timeShift();}
    int             timeIndex()             {return mPublicData->timeIndex();}

    bool            replayed()              {return mPublicData->replayed();}
    QString         consumers()             {return mPublicData->consumers();}

    FbsfDataExchange* mPublicData;

private :

    int                 mIndex; // scalar = -1 else vector index [0,...,N-1]
    bool                mExpanded;
    bool                mChecked;
    QColor              mColor;
    FbsfDataModelItem*  mParent;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Data Model for QML listview (Monitor, plotters)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_INTERFACE FbsfdataModel
        : public QAbstractTableModel
{
    Q_OBJECT    // QML binding
    Q_DISABLE_COPY( FbsfdataModel )

public:

    Q_INVOKABLE bool        isUnresolved(const int &index);
    Q_INVOKABLE bool        isUnresolved(QString aName);
    Q_INVOKABLE bool        isScalar(const int &index);
    Q_INVOKABLE bool        isParameter(const int &index);
    Q_INVOKABLE bool        isConstant(const int &index);
    Q_INVOKABLE bool        isHeadVector(const int &index);
    Q_INVOKABLE bool        isHeadDataVector(const int &index);
    Q_INVOKABLE bool        isHeadTimeDependVector(const int &index);
    Q_INVOKABLE bool        isItemVector(const int &index);
    Q_INVOKABLE bool        isVectorExpanded(const int &index);
    Q_INVOKABLE bool        isRealNumber(const int &index);
    Q_INVOKABLE void        toggleVisible(const int &index);
    Q_INVOKABLE void        updateTableView();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Accessor to set value of unresolved variables from monitor UI
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Q_INVOKABLE void        value(const int &index,QString const &value);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Accessor to get value/name of variables for QML UI
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Q_INVOKABLE QVariant    value(const int &index);
    Q_INVOKABLE QVariant    itemValue(const int &index,const int ind);
    Q_INVOKABLE QVariant    value(QString name);
    Q_INVOKABLE int         modelIndex(QString name);
    Q_INVOKABLE QString     name(const int &index);
    Q_INVOKABLE QString     unit(const int &index);
    Q_INVOKABLE int         size(const int &index);
    Q_INVOKABLE QVariantList history(const int &index);
    Q_INVOKABLE QVariantList historywithfutur(const int &index);
    Q_INVOKABLE QVariantList history(QString name);
    Q_INVOKABLE int         historySize(QString name);
    Q_INVOKABLE int         timeShift(const int &index);
    Q_INVOKABLE int         timeIndex(const int &index);

    Q_INVOKABLE void        check(const int &index,bool value);
    Q_INVOKABLE bool        checked(const int &index);
    Q_INVOKABLE void        color(const int &index,QColor value);
    Q_INVOKABLE QColor      color(const int &index);

    Q_INVOKABLE bool        replayed(const int &index);
    Q_INVOKABLE QString     consumers(const int &index);

    Q_PROPERTY( QVariant producers READ getProducers NOTIFY producersChanged )
    Q_PROPERTY( int replaySteps READ getReplaySteps NOTIFY replayStepsChanged )

    // Time DAte Conversion utility for qml objects
    Q_INVOKABLE QString     getDateFromTime(QString time) {return QDateTime().fromSecsSinceEpoch(time.toDouble()).toString("yyyy MM dd HH:mm:ss");}
    Q_INVOKABLE int         getTimeFromDate(QDate date, QString time) {return QDateTime().fromString(date.toString("yyyy MM dd") + " " + time, "yyyy MM dd HH:mm:ss").toSecsSinceEpoch();}


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const QVariant     getProducers   ( ) const { return m_producerlist; }
    const int          getReplaySteps   ( ) const { return FbsfDataExchange::m_replay_step; }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:
    enum DataRoles {
        ExpandRole = Qt::UserRole + 1,
        CheckedRole,
        NameRole,
        ProducerRole,
        ValueRole,
        DescriptionRole,
        UnitRole
    };
    explicit    FbsfdataModel(QObject *parent = nullptr);
    void        addData(FbsfDataExchange *aData, bool aInsertRow=false);
    void        addProducer(const QString &producer);
    int         rowCount(const QModelIndex & parent = QModelIndex()) const;
    int         columnCount(const QModelIndex &parent = QModelIndex())
                const {Q_UNUSED(parent) return UnitRole - Qt::UserRole + 1;}
    QVariant    data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    QList<FbsfDataModelItem*>& datalist(){return m_datalist;}
    FbsfDataModelItem*         datalist(int aIndex){if (aIndex < m_datalist.count()) return m_datalist[aIndex];
                                                    else {qDebug() << "[ERROR] Index out of bound";return m_datalist[0];}                                                     }
signals :
    void        producersChanged ( );
    void        replayStepsChanged ( );
public slots:
    void        updateValues();         // update monitor values
    void        updateList();           // update monitor data list

protected:
    QHash<int, QByteArray>      roleNames() const;
private:
    QList<FbsfDataModelItem*>   m_datalist;
    QStringList              m_producerlist;
    QHash<QString, int>         h_pendingVectorDeclaration;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Proxy model for filtering
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Accessor to force value of unresolved variables from monitor UI
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QObject *source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(QByteArray filterRole READ filterRole WRITE setFilterRole)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString)
    Q_PROPERTY(FilterSyntax filterSyntax READ filterSyntax WRITE setFilterSyntax)
    Q_ENUMS(FilterSyntax)

public:
    explicit FbsfFilterProxyModel(QObject *parent = 0);

    //  accessor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Q_INVOKABLE void        toggleVisible(const int &idx);
    Q_INVOKABLE int         getRow(const int &idx);
    // filter accessor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    QObject *source() const;
    void setSource(QObject *source);

    QByteArray filterRole() const;
    void setFilterRole(const QByteArray &role);

    QString filterString() const;
    void setFilterString(const QString &filter);

    enum FilterSyntax {RegExp,Wildcard,FixedString };

    FilterSyntax filterSyntax() const;
    void setFilterSyntax(FilterSyntax syntax);

    int count() const;

    void componentComplete();

signals:
    void countChanged();
    void sourceChanged();
protected:
    int                     roleKey(const QByteArray &role) const;
    QHash<int, QByteArray>  roleNames() const;
    bool                    filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    bool        m_complete;
    QByteArray  m_filterRole;
    QString     m_filter;
    QString     m_CurrentProducer;
    int         m_role;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef FBSF_PUBLIC_DATA
#ifdef Q_OS_LINUX
extern FbsfdataModel sListviewDataModel;
#else
Q_DECL_IMPORT FbsfdataModel sListviewDataModel;
#endif
#endif
#endif// ifndef MODE_BATCH
#endif // FBSFPULICDATAMODEL_H
