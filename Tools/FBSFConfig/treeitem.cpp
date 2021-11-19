#include <QStringList>
#include "TreeItem.h"
#include "ItemProperties.h"
#include "ModuleDescriptor.h"
#include <QDebug>
#include <QList>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// for plugin : name=instance, type=module, category=plugin
// for module : name=instance, type=module, category=(fmu,manual,visual)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem::TreeItem(QString aName, QString aType,
                   QString aCategory, QString aModuleType, TreeItem *parent)
{
    mParentItem = parent;
    // set the item descriptor
    mDescriptor << aName;
    mDescriptor << aType;
    mDescriptor << aCategory;
    mDescriptor << aModuleType;

    createItemParamList();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem::TreeItem(QMap<QString, QString> &aXmlParamList,
                   QString aName, QString aType,
                   QString aCategory, QString aModuleType,
                   TreeItem *parent)
{
    mParentItem = parent;

    // set the item descriptor
    mDescriptor << aName;           // name
    mDescriptor << aType;           // config,sequence,module,plugin
    mDescriptor << aCategory;       // module category : fmu, manual, visual
    mDescriptor << aModuleType;     // cpp type

    // fill the parameter list
    if(type()==typeConfig)
    {
        mItemParams.setItemParams(aXmlParamList,sSimulationProperties);
    }
    else if(type()==typeSequence)
    {
        mItemParams.setItemParams(aXmlParamList,sSequenceProperties);
    }
    else if(type()==typeModule)
    {
        if(category()==typePlugin)
        {
            mItemParams.setItemParams(aXmlParamList,sPluginProperties);
        }
        else
        {
            mItemParams.setItemParams(aXmlParamList,
                                      ModuleDescriptor::getModuleTypeProperties(aModuleType));
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem::~TreeItem()
{
    qDeleteAll(mChildItems.begin(),mChildItems.end());
    mChildItems.clear();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Called on item creation and when module type is changed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeItem::createItemParamList()
{
    mItemParams.clear();
    if(type()==typeConfig)
    {
        mItemParams.createItemParams(sSimulationProperties);
    }
    else if(type()==typeSequence)
    {
        mItemParams.createItemParams(sSequenceProperties);
    }
    else if(type()==typeModule)
    {
        if(category()==typePlugin)
        {
            mItemParams.createItemParams(sPluginProperties);
        }
        else if(!moduleType().isEmpty())
        {
            mItemParams.createItemParams(ModuleDescriptor::getModuleTypeProperties(moduleType()));
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString  TreeItem::checkParamList()
{
    QString report;
    QString header;
    QString errorList;

    bool bHasError=false;

    if(type()==typeFork || type()==typePlugins)
        return report;// nothing to check

    if(type()==typeModule && category()==typePlugin)
    {
        header = category() + " " + name() + " : ";
    }
    else
    {
        header = type() + " " + name() ;
        if(!category().isEmpty()) header+= "(" + category() + ") : ";
        else header += " : ";
    }

    // Check if module is abstract
    if(type()==typeModule && category()=="")
    {
        report = header + "\n\tAbstract module,the type must be defined\n";
    }
    else
    {
        // Check every parameter
        QList<QObject*>::const_iterator iter;
        for (iter = getParamList().constBegin();
             iter != getParamList().constEnd(); ++iter)
        {
            // get parameter
            ParamValue& data =*( dynamic_cast<ParamValue*>(*iter));// down cast
            data.hasError(false);

            // skip if optional and value is default
            // special case if date : may be unchanged while format is not
            if(!data.isModified() && data.type()!=ParamValue::typeDate) continue;

            if(data.isInvalid())
            {
                errorList += "\n\t"+data.error();
                bHasError=true;
            }
            // Check if mandatory is valid
//            if(data.mandatory() && data.getValue()=="")
//            {
//                data.error("empty value for " + data.key());
//                errorList += "\n\t"+data.error();
//                data.hasError(true);
//                bHasError=true;
//            }
//            // Check if value is in list
//            if(data.type()==ParamValue::typeStringList)
//            {
//                if(!data.value().toList().contains(data.getValue()))
//                {
//                    data.error("Value not in list for " + data.key());
//                    errorList += "\n\t"+data.error();
//                    data.hasError(true);
//                    data.error("Value not in list for " + data.key());
//                    bHasError=true;
//                }
//            }
//            else if(data.type()==ParamValue::typeDate && !data.value().toString().isEmpty())
//            {
//                QString format=data.unit().isEmpty()?defaultDateFormat:data.unit();
//                QDate isoDate=QDate::fromString(data.value().toString(),format);
//                if(!isoDate.isValid())
//                {
//                    data.error("not a valid date/format for " + data.key()
//                                 +", check format : "+data.value().toString()+"/"+data.unit());
//                    errorList += "\n\t"+data.error();
//                    data.hasError(true);
//                    bHasError=true;
//                }
//            }
//            else
//            {
//                // Check range for value
//                switch (data.defaultType())
//                {
//                case QVariant::Bool   :
//                    if(data.value().toString().toLower()!="true"
//                            &&data.value().toString().toLower()!="false")
//                    {
//                        data.error("not a boolean value for " + data.key());
//                        errorList += "\n\t"+data.error();
//                        data.hasError(true);
//                        bHasError=true;
//                    }
//                    break;
//                case QVariant::Int    :
//                    if (data.value().toInt()<data.minStrict().toInt()
//                            || data.value().toInt()>data.maxStrict().toInt())
//                    {
//                        data.error("value out of range for " + data.key());
//                        errorList += "\n\t"+data.error();
//                        data.hasError(true);
//                        bHasError=true;
//                    }
//                    break;
//                case QVariant::Double :
//                    if (data.value().toDouble()<data.minStrict().toDouble()
//                            || data.value().toDouble()>data.maxStrict().toDouble())
//                    {
//                        data.error("value out of range for " + data.key());
//                        errorList += "\n\t"+data.error();
//                        data.hasError(true);
//                        bHasError=true;
//                    }
//                    break;
//                case QVariant::LongLong :
//                    if (data.value().toLongLong()<data.minStrict().toLongLong()
//                            || data.value().toLongLong()>data.maxStrict().toLongLong())
//                    {
//                        data.error("value out of range for " + data.key());
//                        errorList += "\n\t"+data.error();
//                        data.hasError(true);
//                        bHasError=true;
//                    }
//                    break;
//                default : break;
//                }
//            }
        }
        if(bHasError) report = header + errorList + "\n";
        hasError(bHasError);
    }
    return report;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeItem::appendChild(TreeItem *item)
{
    item->setParentItem(this);
    mChildItems.append(item);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeItem::insertChild(TreeItem *item,int position)
{
    item->setParentItem(this);
    mChildItems.insert(position,item);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeItem::moveChild(int from, int to)
{
    if (to < 0 || to > mChildItems.size())
        return false;
    mChildItems.move(from,to);

    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem *TreeItem::child(int row)
{
    return mChildItems.value(row);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int TreeItem::childCount() const
{
    return mChildItems.count();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int TreeItem::columnCount() const
{
    return mDescriptor.count();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant TreeItem::data(int column) const
{
    return mDescriptor.value(column);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QList<QObject *>& TreeItem::getParamList()
{
    return mItemParams;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeItem::setParentItem(TreeItem *parentItem)
{
    mParentItem = parentItem;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem *TreeItem::parentItem()
{
    return mParentItem;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int TreeItem::row() const
{
    if (mParentItem)
        return mParentItem->mChildItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= mDescriptor.size())
        return false;

    mDescriptor[column] = value;
    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// currently not used
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeItem::insertChildren(int position, int count, QVector<QVariant>& data)
{
    Q_UNUSED(data)
    if (position < 0 || position > mChildItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        TreeItem *item = new TreeItem(*this);
        mChildItems.insert(position, item);
    }
    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > mChildItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete mChildItems.takeAt(position);

    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Assignement operator
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem &TreeItem::operator =(const TreeItem &other)
{
    if(this==&other) return *this;
    return *(new TreeItem(other));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// copy constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem::TreeItem(const TreeItem& other)
{
    for (TreeItem* child : other.mChildItems)
    {
        TreeItem* itemBack=new TreeItem(*child);
        appendChild(itemBack);
    }
    mDescriptor=other.mDescriptor;      // QVector<QVariant>
    mParentItem=other.mParentItem;      // TreeItem *
    mItemParams=other.mItemParams;      // copy params
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// get xml formatted string of item parameters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeItem::getXmlItemData(QString& aXmlConfig, int level)
{
    if(name()=="root") return;// no parameters

    QString tab;for(int i=0;i<level;i++) tab+="\t";
    QList<QObject*>::const_iterator iter;
    for (iter = getParamList().constBegin();
         iter != getParamList().constEnd(); ++iter)
    {
        // get parameter
        ParamValue& data =*( dynamic_cast<ParamValue*>(*iter));// down cast


        // skip if value is optional and equal to default
        // special case if date : may be unchanged while format is not
        if(!data.isModified() && data.type()!=ParamValue::typeDate) continue;

        if(data.type()==ParamValue::typeStringList)
        {
            if (data.currentText().isEmpty()) continue;
            aXmlConfig+= tab +"<"+ data.key()+">"+data.currentText()+"</"+ data.key()+">";
        }
        else if(data.type()==ParamValue::typeDate)
        {
            if(!data.value().toString().isEmpty())// set date if not empty
                aXmlConfig+= tab +"<"+ data.key()+">"+data.value().toString()+"</"+ data.key()+">\n";
            if(!data.unit().isEmpty()) // set format if not empty
                aXmlConfig+= tab +"<dateFormat>"+data.unit()+"</dateFormat>";
        }
        else
        {
            if(data.value().toString().isEmpty()) continue;
            switch (data.defaultType())
            {
            case QVariant::Bool   : {
                aXmlConfig+= tab +"<"+ data.key()+">"+data.value().toString()+"</"+ data.key()+">";
                break;}
            case QVariant::Int    : {
                aXmlConfig+= tab +"<"+ data.key()+">"+QString::number(data.value().toInt())+"</"+ data.key()+">";
                break;}
            case QVariant::Double : {
                aXmlConfig+= tab +"<"+ data.key()+">"+QString::number(data.value().toDouble())+"</"+ data.key()+">";
                break;}
            case QVariant::String : {
                aXmlConfig+= tab +"<"+ data.key()+">"+data.value().toString()+"</"+ data.key()+">";
                break;}
            case QVariant::LongLong    : {
                aXmlConfig+= tab +"<"+ data.key()+">"+QString::number(data.value().toLongLong())+"</"+ data.key()+">";
                break;}
            default : qDebug() << __FUNCTION__<< name()
                               << "Unknown type for key :" << data.key();break;
            }
        }
        //        else if(data.type()==ParamValue::typeChoiceList)
        //        {
        //            qDebug() << data.key();
        //            for (auto val:data.value().toStringList())
        //                qDebug() << data.value();
        //        }
        aXmlConfig+="\n";
    }
}


