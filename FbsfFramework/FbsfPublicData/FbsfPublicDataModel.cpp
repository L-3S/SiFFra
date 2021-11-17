#include "FbsfPublicDataModel.h"
#include "FbsfPublicData.h"

#ifndef MODE_BATCH
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static instance of public data model to QML
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfdataModel *FbsfdataModel::sListviewDataModel = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfdataModel
    \brief Public memory acces as a ListModel for QML
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static QString tagUnlinked("unlinked");
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfdataModel::FbsfdataModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_producerlist.append("");
    m_producerlist.append("unlinked");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// add data to view model
//
// secial case for vector which are declared import before export
// The size (line number) are known later and must be inserted.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::addData(FbsfDataExchange *aData, bool aInsertRow)
{
    if (aData->Class()==FbsfDataClass::cScalar)
    {   // Scalar ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        FbsfDataModelItem* item= new FbsfDataModelItem(aData);
        m_datalist.append(item);
        endInsertRows();
    }
    else
    {   // Vector ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        int numrow=0;
        FbsfDataModelItem* vector=NULL;
        if (aInsertRow)
        {   // Case of delayed export declaration
            if(h_pendingVectorDeclaration.contains(aData->name()))
            {
                numrow=h_pendingVectorDeclaration[aData->name()];
                vector=m_datalist[numrow];
                // update row number for followings
                QHash<QString, int>::iterator i;
                for (i = h_pendingVectorDeclaration.begin();
                     i != h_pendingVectorDeclaration.end(); ++i)
                        if (i.value()>numrow) i.value()+=aData->size();
            }
            else return; // error case
        }
        else // append mode
        {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());
            vector= new FbsfDataModelItem(aData);
            m_datalist.append(vector);
            endInsertRows();
            numrow=rowCount();
            // declaration for imported
            if (aData->size()==0) h_pendingVectorDeclaration[aData->name()]=numrow-1;
        }
        // insert vector item rows
        for (int i=0;i<aData->size();i++)
        {
            beginInsertRows(QModelIndex() , numrow+i, numrow+i);
            FbsfDataModelItem* vectorItem= new FbsfDataModelItem(aData,vector,i);
            if (aInsertRow)
                m_datalist.insert(numrow+i+1,vectorItem);
            else
                m_datalist.append(vectorItem);
            endInsertRows();
        }
        updateList();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// add producer to view model
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::addProducer(const QString& producer)
{
    if(!m_producerlist.contains(producer))
            m_producerlist.append(producer);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfdataModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_datalist.count();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant FbsfdataModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
      return QVariant();

    if (index.row() < 0 || index.row() >= m_datalist.count())
        return QVariant();

    FbsfDataModelItem* data = m_datalist[index.row()];

    switch (role)
    {
    case ExpandRole:        return data->isHeadVector()?
                                    data->isVectorExpanded() ? "-":"+"
                                    :"";
    case CheckedRole:       return data->checked();
    case NameRole:          return data->name();
    case ProducerRole:      return data->index()>=0? "": data->producer().isEmpty()?
                                                         tagUnlinked:data->producer();
    case ValueRole:         return data->isHeadVector() ? QVariant(): data->value();
    case DescriptionRole:   return data->index()>=0? "": data->description();
    case UnitRole:          return data->index()>=0? "": data->unit();
    default :               return QVariant();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QHash<int, QByteArray> FbsfdataModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ExpandRole]       = "expand";
    roles[CheckedRole]      = "checked";
    roles[NameRole]         = "name";
    roles[ProducerRole]     = "producer";
    roles[ValueRole]        = "value";
    roles[DescriptionRole]  = "description";
    roles[UnitRole]         = "unit";
    return roles;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::updateValues()
{
    emit dataChanged(index(0,0),index(rowCount()-1,0),QVector<int>(1,ValueRole));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::updateList()
{
    QVector<int> roles;
    roles.append(ExpandRole);
    roles.append(CheckedRole);
    roles.append(NameRole);
    roles.append(ProducerRole);
    roles.append(UnitRole);
    roles.append(DescriptionRole);
    emit dataChanged(index(0,0),index(rowCount()-1,0),roles);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::updateTableView()
{
    updateValues();
    emit layoutChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Not produced variable by Index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isUnresolved(const int &index)
{
    return (index>=0 ? datalist(index)->isUnresolved():true);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Not produced variable by Name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isUnresolved(QString aName)
{
    int index = modelIndex(aName);
    return (index>=0 ? datalist(index)->isUnresolved():true);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scalar
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isScalar(const int &index)
{
    return (index>=0 ? datalist(index)->isScalar():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scalar
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isParameter(const int &index)
{
    return (index>=0 ? datalist(index)->isParameter():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scalar
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isConstant(const int &index)
{
    return (index>=0 ? datalist(index)->isConstant():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Vector header
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isHeadVector(const int &index)
{
    return (index>=0 ? datalist(index)->isHeadVector():0);
}
bool FbsfdataModel::isHeadDataVector(const int &index)
{
    return (index>=0 ? datalist(index)->isHeadDataVector():0);
}
bool FbsfdataModel::isHeadTimeDependVector(const int &index)
{
    return (index>=0 ? datalist(index)->isHeadTimeDependVector():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Vector Item
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isItemVector(const int &index)
{
    return (index>=0 ? datalist(index)->isItemVector():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Vector Item
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isVectorExpanded(const int &index)
{
    return (index>=0 ? datalist(index)->isVectorExpanded():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Expand or collapse vector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::toggleVisible(const int &index)
{
    if (isHeadVector(index) && isUnresolved(index))return;
    datalist(index)->toggleVisible();
    emit layoutChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// force value of unresolved variables from monitor UI
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::value(const int &index,QString const &value)
{
    if (index<0)return;
    switch (datalist(index)->Type())
    {
    case cReal:
        datalist(index)->value(value.toFloat());
        break;
    case cInteger :
        datalist(index)->value(value.toInt());
        break;
    default : break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  for QML format number in monitor UI
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::isRealNumber(const int &index)
{
    return (index>=0? datalist(index)->Type() == cReal:0 );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  for QML display in plotter
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get name by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfdataModel::name(const int &index)
{
    return (index>=0? datalist(index)->name():"");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get unit by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfdataModel::unit(const int &index)
{
    return (index>=0? datalist(index)->unit():"");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get value by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant FbsfdataModel::value(const int &index)
{
    return (index>=0? datalist(index)->value():QVariant());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get vector item value by indexes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant FbsfdataModel::itemValue(const int &index,const int aInd)
{
    return (index>=0? datalist(index)->itemValue(aInd):QVariant());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Not relevant to adapt with vector
//  Get value by name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant FbsfdataModel::value(QString name)
{
    if (FbsfDataExchange::sPublicDataMap.contains(name))
    {
        FbsfDataExchange *publicAddress;
        publicAddress = FbsfDataExchange::sPublicDataMap.value(name);
        return publicAddress->value();
    }
    else
    {
        return QVariant();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Get index by name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfdataModel::modelIndex(QString name)
{
    for (int i=0;i < m_datalist.length();i++)
        if(m_datalist[i]->name() == name) return i;
    return -1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get size by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfdataModel::size(const int &index)
{
    return (index>=0? datalist(index)->Size():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Get historical data by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariantList FbsfdataModel::history(const int &index)
{
    return (index>=0? datalist(index)->history():QVariantList());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Get past and futur data by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariantList FbsfdataModel::historywithfutur(const int &index)
{
    return (index>=0? datalist(index)->historywithfutur():QVariantList());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Get historical data by name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariantList FbsfdataModel::history(QString name)
{
    if (FbsfDataExchange::sPublicDataMap.contains(name))
    {
        FbsfDataExchange *publicAddress;
        publicAddress = FbsfDataExchange::sPublicDataMap.value(name);
        return publicAddress->history();
    }
    else
    {
        qWarning() << name <<  "history is empty";
        return QVariantList();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Get historical size by name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfdataModel::historySize(QString name)
{
    if (FbsfDataExchange::sPublicDataMap.contains(name))
    {
        FbsfDataExchange *publicAddress;
        publicAddress = FbsfDataExchange::sPublicDataMap.value(name);
        return publicAddress->historySize();
    }
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Get timeShift by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfdataModel::timeShift(const int &index)
{
    return (index>=0? datalist(index)->timeShift():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Get timeIndex by index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfdataModel::timeIndex(const int &index)
{
    return (index>=0? datalist(index)->timeIndex():0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::check(const int &index,bool value)
{
    if (index>=0)  datalist(index)->checked(value);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::checked(const int &index)
{
    return (index>=0 ? datalist(index)->checked():false);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfdataModel::color(const int &index,QColor value)
{
    if (index>=0) datalist(index)->color(value);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QColor FbsfdataModel::color(const int &index)
{
    return (index>=0 ? datalist(index)->color():QColor());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfdataModel::replayed(const int &index)
{
    return (index>=0 ? datalist(index)->replayed():false);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfdataModel::consumers(const int &index)
{
    return (index>=0 ? datalist(index)->consumers():QString());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class for filtering Listview
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfFilterProxyModel::FbsfFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_complete(false)
    , m_filterRole(0)
    , m_filter()
    , m_CurrentProducer()
    , m_role(0)
{
    setSourceModel(FbsfdataModel::sFactoryListviewDataModel());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfFilterProxyModel::count() const
{
    return rowCount();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QObject *FbsfFilterProxyModel::source() const
{
    return sourceModel();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfFilterProxyModel::setSource(QObject *source)
{
    setSourceModel(qobject_cast<QAbstractTableModel *>(source));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QByteArray FbsfFilterProxyModel::filterRole() const
{
    return m_filterRole;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfFilterProxyModel::setFilterRole(const QByteArray &role)
{
    if (m_filterRole != role)
    {
        m_filterRole = role;
        if (m_complete)
            QSortFilterProxyModel::setFilterRole(roleKey(role));
        // save it as integer
        m_role=roleKey(m_filterRole);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfFilterProxyModel::filterString() const
{
    return filterRegExp().pattern();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfFilterProxyModel::setFilterString(const QString &filter)
{
    setFilterRegExp(QRegExp(filter, filterCaseSensitivity(),
                            static_cast<QRegExp::PatternSyntax>(filterSyntax())));

    m_filter=filterRegExp().pattern();
    if(m_role==FbsfdataModel::ProducerRole) m_CurrentProducer=m_filter;
    invalidateFilter();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfFilterProxyModel::FilterSyntax FbsfFilterProxyModel::filterSyntax() const
{
    return static_cast<FilterSyntax>(filterRegExp().patternSyntax());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfFilterProxyModel::setFilterSyntax(FbsfFilterProxyModel::FilterSyntax syntax)
{
    setFilterRegExp(QRegExp(filterString(), filterCaseSensitivity(),
                            static_cast<QRegExp::PatternSyntax>(syntax)));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfFilterProxyModel::componentComplete()
{
    m_complete = true;
    if (!m_filterRole.isEmpty())
        QSortFilterProxyModel::setFilterRole(roleKey(m_filterRole));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfFilterProxyModel::roleKey(const QByteArray &role) const
{
    QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> it(roles);
    while (it.hasNext()) {
        it.next();
        if (it.value() == role)
            return it.key();
    }
    return -1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QHash<int, QByteArray> FbsfFilterProxyModel::roleNames() const
{
    if (QAbstractItemModel *source = sourceModel())
        return source->roleNames();
    return QHash<int, QByteArray>();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfFilterProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent )const
{
    QModelIndex sourceIndex = FbsfdataModel::sFactoryListviewDataModel()->index(sourceRow, 0, sourceParent);
    FbsfDataModelItem* item= FbsfdataModel::sFactoryListviewDataModel()->datalist()[sourceIndex.row()];
    FbsfDataExchange* data=item->mPublicData;

    // filter with producer first
    if(!m_CurrentProducer.isEmpty())
    {
        if (m_CurrentProducer==tagUnlinked)
            {if (!data->m_producer.isEmpty())return false;}
        else
            {if (m_CurrentProducer!=data->m_producer)return false;}
    }
    if (item->isItemVisible())// filter only expanded items
    {
        QRegExp rx = filterRegExp();// REGEXP
        if (rx.isEmpty()) {return true;}// REGEXP
        //if (m_filter.isEmpty() )return true;
        switch (m_role)
        {
        case FbsfdataModel::NameRole :
            if (rx.exactMatch(data->m_tag))return true; ;// REGEXP
            //if (data->m_tag.contains(m_filter)) return true ;
            break;
        case FbsfdataModel::ProducerRole :
            if (m_filter==tagUnlinked && data->m_producer.isEmpty())
                return true;
            else
                if (data->m_producer==m_filter) return true;
            break;
        case FbsfdataModel::UnitRole :
            if (rx.exactMatch(data->m_unit))return true; ;// REGEXP
            //if (data->m_unit.contains(m_filter)) return true;
            break;
        case FbsfdataModel::DescriptionRole :
            if (rx.exactMatch(data->m_description))return true; ;// REGEXP
            //if (data->m_description.contains(m_filter)) return true;
            break;
        default : break;
        }
    }
    return false;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfFilterProxyModel::getRow(const int &idx)
{
    return mapToSource(index(idx,0)).row();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void  FbsfFilterProxyModel::toggleVisible(const int &idx)
{
    FbsfdataModel::sFactoryListviewDataModel()->toggleVisible(mapToSource(index(idx,0)).row());
}
#endif
