#include "TreeItem.h"
#include "TreeModel.h"
#include "UndoManager.h"
#include "ItemParams.h"

#include "FbsfConfiguration.h"

#include <QStringList>
#include <QUndoStack>
#include <QUndoCommand>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <sstream>
#include <random>
#include <string>
//#define TMP_NAME
static QString typeRootFork("rootFork");
static QString typeRootSequence("rootsequence");

QList<TreeItem *> TreeModel::sClipBoard; // shared btw the 2 editors

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeModel::TreeModel( QObject *parent)
    : QAbstractItemModel(parent)
{
    m_roleNameMapping[RoleName] = "name";
    // descriptor part
    m_roleNameMapping[RoleType] = "type";
    m_roleNameMapping[RoleCategory] = "category";
    m_roleNameMapping[RoleModuleType] = "moduleType";
    // gui roles
    m_roleNameMapping[RoleSelected] = "selected";
    m_roleNameMapping[RoleHasError] = "hasError";
    m_roleNameMapping[RoleParamList] = "params";
    // root
    rootItem = new TreeItem("root",typeRootSequence);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeModel::~TreeModel()
{
    delete rootItem;
}
//////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~ Configuration xml to/from tree model ~~~~~~~~~~~~~~~~~~
//////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ : set up the model from xml configuration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QUrl TreeModel::newModelData(QString aName)
{
    emit layoutAboutToBeChanged();

    // insert a config node
    auto config = new TreeItem(aName,typeConfig);
    rootItem->appendChild(config);
    configItem=config;

    mHasPluginList=false; // reset plugin list

    //insert an initial fork (main sequence)
    TreeItem* fork = new TreeItem(typeRootFork,
                                  typeFork,typeRootFork);
    configItem->appendChild(fork);

    // insert a main sequence node
    auto sequence = new TreeItem("main",typeSequence);

    fork->appendChild(sequence);

    emit layoutChanged();

    // update the config item with default name
    QUrl filePath=QUrl::fromLocalFile(QDir::currentPath()+"/"+aName+".xml");
    setConfigUrl(filePath);
    loaded(true);

    return filePath;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ : set up the model from xml configuration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::setupModelData(QString aFilename)
{
    FbsfConfiguration xmlConfig;
    if(xmlConfig.parseXML( aFilename)==-1)return;

    emit layoutAboutToBeChanged();

    // Insert the plugin list item
    if(xmlConfig.pluginList().Models().count()>0)
        readPlugins(xmlConfig.pluginList());

    // Insert the config item
    TreeItem* fork=readSimulation(xmlConfig);

    // Insert the sequence items
    QList<FbsfConfigSequence>::iterator iSeq;
    for (iSeq = xmlConfig.Sequences().begin();
         iSeq != xmlConfig.Sequences().end(); ++iSeq)
    {
        // create the sequence item
        readSequence(fork,*iSeq);
    } // end Sequences
    emit layoutChanged();

    // update the config item with opened file basename
    setConfigUrl(QUrl::fromLocalFile(aFilename));
    loaded(true);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Clear the model data
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::clearModelData()
{
    beginResetModel();
    qDeleteAll(rootItem->childItems().begin(),rootItem->childItems().end());
    rootItem->childItems().clear();
    endResetModel();
    loaded(false);
    setConfigUrl(QUrl());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Check the model data
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const QString TreeModel::checkModelData()
{
    // traverse the tree model to check module and plugin items
    QString report;
    checkItemParams(rootItem,report);

    if(!report.isEmpty())
    {
        hasError(true);
    }
    else
    {
        hasError(false);
    }
    emit layoutChanged();// display/hide error icon

    return report;
}
//~~~~~~~ recursive check of module and plugin items ~~~~~~~~~~
void TreeModel::checkItemParams(TreeItem* item,QString& aReport)
{
    if(item != rootItem) aReport+=item->checkParamList();
    for (int i=0;i<item->childCount();i++)
    {
        checkItemParams(item->child(i),aReport);
    }
}
//~~~~~~~ check module and plugin items ~~~~~~~~~~
void TreeModel::checkItemParams(QModelIndex aIndex)
{
    TreeItem *item = getItem(aIndex);
    item->checkParamList();
    emit layoutChanged();// display/hide error icon
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get the simulation description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem *TreeModel::readSimulation(FbsfConfiguration &aXmlConfig)
{
    // insert a config node
    QVector<QVariant> descriptor;
    descriptor << aXmlConfig.Name() << typeConfig << "";
    auto config = new TreeItem(aXmlConfig.Simulation(),
                               aXmlConfig.Name(),typeConfig);
    rootItem->appendChild(config);
    configItem=config;

    //insert a fork item
    TreeItem* fork = new TreeItem(typeRootFork,
                                  typeFork,typeRootFork);
    configItem->appendChild(fork);

    return  fork;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get the sequence description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::readSequence(TreeItem* parent,FbsfConfigSequence& aSeq,bool isSub)
{
    // insert a sequence node
    QString name= aSeq.Descriptor().value("name",QString("subSeq%1").arg(sequenceAutoIndex));

    auto sequence = new TreeItem(aSeq.Descriptor(),name,
                                 typeSequence,(isSub?typeSubSequence:""));
    parent->appendChild(sequence);
    sequenceAutoIndex++;

    // create the module items
    readModules(sequence,aSeq.Models(), aSeq.Nodes());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get the plugin list description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::readPlugins(FbsfConfigSequence& aPluginList)
{
    // insert a plugin list node
    auto pluginList = new TreeItem("plugin list",typePluginList);
    rootItem->appendChild(pluginList);
    mHasPluginList=true; // only one instance allowed

    QList<QMap<QString,QString>>::iterator iMod;
    for (iMod = aPluginList.Models().begin();
         iMod != aPluginList.Models().end(); ++iMod)
    {
        QMap<QString,QString> vMap = *iMod;
        // create the plugin items
        auto plugin = new TreeItem(vMap,vMap.value("name"),
                                   typeModule,typePlugin);
        pluginList->appendChild(plugin);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get the modules description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::readModules(TreeItem* parent,
                            QList<QMap<QString, QString> > &aModels,
                            QList<FbsfConfigNode> & aNodes)
{
    QList<QMap<QString,QString>>::iterator iMod;
    for (iMod = aModels.begin(); iMod != aModels.end(); ++iMod)
    {
        QMap<QString,QString> vMap = *iMod;

        //~~~~~~~~~~~~ Check if type is node to fork ~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (vMap.value("type")=="node")
        {
            //static int n = 0;n++;
            if (aNodes.size() < 1) continue;
            //insert a fork
            auto fork = new TreeItem(QString("fork%1").arg(forkAutoIndex),
                                     typeFork);
            forkAutoIndex++;

            parent->appendChild(fork);
            for(int i = 0; i < aNodes[0].Sequences().size(); i++)
            {
                FbsfConfigSequence sequence=aNodes[0].Sequences()[i];
                readSequence(fork,sequence,true);
            }
            aNodes.pop_front();
        }
        else
        {
            // insert a module item
            QString vModuleType=vMap.value("type","")==typeFmu?
                        typeFmu:vMap.value("module","");
            auto module = new TreeItem(vMap,
                                       vMap.value("name"),typeModule,
                                       vMap.value("type",""),
                                       vMap.value("module",vModuleType)
                                       );
            parent->appendChild(module);
        }
    }// end Models
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Get the tree model as an xml tree
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::getXmlConfig(QString& aXmlConfig)
{
    aXmlConfig = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    getXmlSubtree(aXmlConfig,rootItem,0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Recursive construct of the tree
// translate tree model tag to xml tag
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::getXmlSubtree(QString& aXmlConfig,TreeItem* item, int level)
{
    QString tag;
    if(item->name()=="root")              tag="Items";
    else if(item->type()==typeConfig)     tag="simulation";
    else if(item->type()==typePluginList)    tag="PluginsList";
    else if(item->type()==typeFork)
        tag=(item->parentItem()->type()==typeSequence?"node":"sequences");
    else if(item->type()==typeSequence)
        tag=(item->category()==typeSubSequence?"SubSequence":"sequence");
    else if(item->type()==typeModule)
        tag=(item->category()==typePlugin?"Plugin":"model");
    // indentation
    QString tab;for(int i=0;i<level;i++) tab+="\t";++level;

    // set the begin tag
    aXmlConfig += tab + "<"+tag+">\n";

    // set the "name" attribute
    if(item->type()==typeSequence|| item->type()==typeModule)
        aXmlConfig += tab+ "\t<name>" + item->name()+"</name>\n";
    // set the "type" and "module" attributes
    if(item->type()==typeModule &&item->category()!=typePlugin)
    {
        aXmlConfig += tab+ "\t<type>" + item->category()+"</type>\n";
        if(item->category()!=typeFmu)
            aXmlConfig += tab+ "\t<module>" + item->moduleType()+"</module>\n";
    }
    // get item parameters
    item->getXmlItemData(aXmlConfig,level);

    // specific case for config = fork with one sequence
    if(item->type()==typeConfig)
    {
        aXmlConfig+= tab + "</"+tag+">\n";// close the "simulation" section
        level--;
    }
    // set inner tag
    if(tag=="sequence"||tag=="SubSequence")
    {
        aXmlConfig += tab + "\t<models>\n";++level;
    }
    else if(tag=="node")
    {
        aXmlConfig += tab + "\t<SubSequences>\n";++level;
    }

    // traverse the tree recusively
    for (int i=0;i<item->childCount();i++)
    {
        getXmlSubtree(aXmlConfig,item->child(i),level);
    }
    // set the additional end tag
    if(tag=="node")
    {
        aXmlConfig += tab + "\t</SubSequences>\n";
    }
    else if(tag=="sequence"||tag=="SubSequence")
    {
        aXmlConfig += tab + "\t</models>\n";
    }

    if(!(item->type()==typeConfig))
        aXmlConfig+= tab + "</"+tag+">\n";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString TreeModel::configName()
{
    if(configItem!=nullptr)
        return configItem->name();
    else
        return QString();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::setConfigName(const QString& aName)
{
    if(configItem!=nullptr)
    {
        configItem->name(aName);
        emit configNameChanged();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::setConfigUrl(const QUrl& aFileUrl)
{
    mConfigUrl=aFileUrl;
    if(aFileUrl.isEmpty()) return;

    QFileInfo filename(mConfigUrl.toLocalFile());
    emit configUrlChanged();

    setConfigName(filename.baseName());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Recursive print of the tree
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::print(TreeItem* item, int level)
{
    QString tab;
    for(int i=0;i<level;i++) tab+="\t";
    qDebug().noquote() << tab << item->type() << item->name() ;
    ++level;
    for (int i=0;i<item->childCount();i++)
    {
        print(item->child(i),level);
    }
}
//////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~ Edition of the tree model ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//////////////////////////////////////////////////////////////////////

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : fork item case management
// idx : target index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::forkItem(const QModelIndex &idx)
{
    if (!idx.isValid()) return;

    TreeItem *item = getItem(idx);
    if(item==nullptr)return;

    if(item->type()==typeFork )
    {   // append one sequence to the fork item
        beginInsertRows(idx,item->childCount(),item->childCount());
        insertSequence(idx);
        endInsertRows();
    }
    else if(item->type()==typeModule)
    {   // fork from item with 2 sequences
        beginInsertRows(idx.parent(),item->row()+1,item->row()+1);
        forkItem(idx.parent(),item->row()+1);
        endInsertRows();
    }
    else if(item->type()==typeSequence || item->type()==typeConfig)
    {   // fork from sequence or config with 2 sequence
        beginInsertRows(idx,0,0);
        forkItem(idx,0);
        endInsertRows();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : fork item at position (undoable)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::forkItem(const QModelIndex aParentIndex, int position)
{
    // insert a fork
    auto fork = new TreeItem(QString("fork%1").arg(forkAutoIndex),
                             typeFork);
    forkAutoIndex++;
#if true
    // insert 2 default subsequences
    for(int j=0;j<2;j++)
    {
        // Insert a new sequence at end
        auto sequence = new TreeItem(typeSequence,
                                     typeSequence,typeSubSequence);
        fork->appendChild(sequence);
    }
#endif

    insertItem(aParentIndex,fork,position);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : insert a sequence at bottom (undoable)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::insertSequence(const QModelIndex aParentIndex)
{
    // Append a new sequence at bottom
    auto sequence = new TreeItem(typeSequence,
                                 typeSequence);

    TreeItem* parent=getItem(aParentIndex);
    insertItem(aParentIndex,sequence,parent->childCount());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ : generic insertion of item at index
// use for item copy/paste
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::insertItem(const QModelIndex aParentIndex,
                           TreeItem* item, int position)
{
    QUndoCommand *insertItemCmd = new insertItemCommand(*this,aParentIndex,item,position);
    mUndoManager.record(insertItemCmd);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : insert a module at position (undoable)
// used for new module creation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::insertModule(const QModelIndex &idx)
{
    if (!idx.isValid()) return;

    TreeItem *itemTarget = getItem(idx);
    TreeItem *parent=itemTarget->parentItem();

    //QString vName= QString("abstract%1").arg(moduleAutoIndex);

    QString vType=typeModule;
    QString vCategory=(itemTarget->type()==typePluginList||parent->type()==typePluginList?
                           typePlugin:""); // plugin else no category

    QString vName= vCategory==typePlugin?"Plugin":"Module";

    if( itemTarget->type()==typeConfig
            ||itemTarget->type()==typeSequence
            || itemTarget->type()==typePluginList)
    {
        // insert if target type is config,sequence,plugin list
        auto child = new TreeItem(vName,vType,vCategory);
        beginInsertRows(idx,0,0);
        insertItem(idx,child,0);
    }
    else if(parent->type()==typeConfig
            || parent->type()==typeSequence
            || parent->type()==typePluginList)
    {
        // insert if item type is module
        auto child = new TreeItem(vName,vType,vCategory);
        beginInsertRows(idx.parent(),idx.row()+1,idx.row()+1);
        insertItem(idx.parent(),child,idx.row()+1);
    }
    else
    {
        qDebug() << __FUNCTION__<< "Parent not candidate" << parent->type();
        return;
    }
    endInsertRows();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : add the plugins list
// plugin list item is added as first child of rootItem
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::addPluginList()
{
    if(mHasPluginList) return;

    auto pluginList = new TreeItem("plugin list", typePluginList);

    // insert a plugin list node
    beginInsertRows(QModelIndex(),0,0);
    insertItem(QModelIndex(),pluginList,0);
    endInsertRows();

    mHasPluginList=true; // only one instance allowed
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// UNUSED QML : remove the plugins list
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::removePluginList()
{
    mHasPluginList=false; // only one instance allowed
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : remove item at index (NOT undoable)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::removeItem(const QModelIndex &index)
{
    if (!index.isValid()) return;
    TreeItem *item = getItem(index);
    if(item!=nullptr && item->type()==typeConfig) return; // not enabled
    removeRow(index.row(), index.parent());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : move to direction item at index (undoable)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::moveItem(const QModelIndex &index,int dir)
{
    if (!index.isValid()) return;
    int destIndex;
    if(dir==1) destIndex=index.row()+2;
    else destIndex=index.row()-1;

    beginMoveRows(index.parent(),index.row(),index.row(), index.parent(),destIndex);
    QUndoCommand *moveItemCmd = new moveItemCommand(*this,index,destIndex-(dir==1?1:0));
    mUndoManager.record(moveItemCmd);
    endMoveRows();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///////////////////// selection management ///////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : selection management (undoable)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::canCutOrCopy()
{
    for(int i=mSelectedIndices.count()-1;i>=0;i--)
    {
        QModelIndex idx=mSelectedIndices[i];
        if (!idx.isValid()) return false;
        TreeItem *item = getItem(idx);
        if(item!=nullptr &&
                (item->type()==typeConfig
                 || item->category()==typeRootFork)) return false; //Config not enabled
    }
    return (mSelectedIndices.count()!=0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::canPaste()
{
    bool flag=false;
    if (sClipBoard.count()!=0 && mSelectedIndices.count()==1)
    {
        flag=true;
        // Check if at least on item to paste is not allowed
        TreeItem *itemTarget = getItem(mSelectedIndices[0]);
        for(auto itemPaste:qAsConst(sClipBoard))
        {
            // can't paste Config
            if (itemPaste->type()==typeConfig) return false;
            // only pluginList allowed if target is config
            else if((itemTarget->type()==typeConfig && (itemPaste->type()!=typePluginList || hasPluginList()))) return false;
            // only sequence allowed if target is fork
            else if(itemTarget->type()==typeFork && itemPaste->type()!=typeSequence) return false;
            // only module and subfork allowed if target is sequence
            else if(itemTarget->type()==typeSequence)
            {
                if ( (itemPaste->type()!=typeModule || itemPaste->category()==typePlugin)
                   &&(itemPaste->type()!=typeFork || itemPaste->name()==typeRootFork) ) return false;// could paste in itself
            }
            // only module and subfork allowed if target is module
            else if(itemTarget->type()==typeModule && itemTarget->category()!=typePlugin)
            {
                if((itemPaste->type()!=typeModule || itemPaste->category()==typePlugin)
                && (itemPaste->type()!=typeFork || itemPaste->name()==typeRootFork)) return false;
            }
            // only plugin if target is plugin list
            else if(itemTarget->type()==typePluginList && itemPaste->category()!=typePlugin) return false;
            // only plugin if target is plugin
            else if(itemTarget->category()==typePlugin && itemPaste->category()!=typePlugin) return false;
        }
    }
    return flag;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::removeSelection()
{
     mUndoManager.beginMacro();// direct call to avoid crash

    for(int i=mSelectedIndices.count()-1;i>=0;i--)
    {
        QModelIndex idx=mSelectedIndices[i];
        if (!idx.isValid()) return;
        TreeItem *item = getItem(idx);
        if(item!=nullptr &&
                (item->type()==typeConfig
                 || item->category()==typeRootFork)) continue; // not enabled

        QUndoCommand *removeItemCmd = new removeItemCommand(*this,idx);
        mUndoManager.record(removeItemCmd);
    }
    mSelectedIndices.clear();// empty selection

    mUndoManager.endMacro();// direct call to avoid crash
    emit canCutOrCopyChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : selection management (undoable)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::cutSelection()
{
    sClipBoard.clear();
    isClipBoardCopy=false;

    for(int i=mSelectedIndices.count()-1;i>=0;i--)
    {
        QPersistentModelIndex idx=mSelectedIndices[i];
        if(idx.isValid()) sClipBoard.append(cloneItem(getItem(idx))); // store a copy
    }
    removeSelection();
    emit canPasteChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::copySelection()
{
    sClipBoard.clear();
    isClipBoardCopy=true;
    for(int i=mSelectedIndices.count()-1;i>=0;i--)
    {
        QPersistentModelIndex idx=mSelectedIndices[i];
        sClipBoard.append(getItem(idx)); // store item
    }
    emit canPasteChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::pasteSelection(const QModelIndex &index)
{
    if(!canPaste()) return;
    beginMacro();
    if (!index.isValid()) return;
    TreeItem *itemTarget = getItem(index);
    for(auto item:qAsConst(sClipBoard))
    {
        TreeItem* itemPaste=cloneItem(item);
        if(itemPaste==nullptr) continue;// case Config
        if(itemTarget->type()==typeConfig)
        {// only PluginList allowed if target is config
            if(itemPaste->type()==typePluginList && !hasPluginList())
                insertItem(index.parent(),itemPaste,0);
        }
        else
            if(itemTarget->type()==typeFork)
            {// only sequence allowed if target is fork
                if(itemPaste->type()==typeSequence)
                    insertItem(index,itemPaste,itemTarget->childCount());
            }
            else if(itemTarget->type()==typeSequence)
            {// only module and subfork allowed if target is sequence
                if(itemPaste->type()==typeModule && itemPaste->category()!=typePlugin)
                    insertItem(index,itemPaste,0);
                else if(itemPaste->type()==typeFork
                        && itemPaste->name()!=typeRootFork)// could paste in itself
                    insertItem(index,cloneItem(itemPaste),0);
            }
            else if(itemTarget->type()==typeModule && itemTarget->category()!=typePlugin)
            {// only module and subfork allowed if target is module
                if((itemPaste->type()==typeModule && itemPaste->category()!=typePlugin)
                        ||(itemPaste->type()==typeFork && itemPaste->name()!=typeRootFork))
                    insertItem(index.parent(),itemPaste,itemTarget->row()+1);
            }
            else if(itemTarget->type()==typePluginList)
            {// only plugin if target is plugin list
                if(itemPaste->category()==typePlugin)
                    insertItem(index,itemPaste,0);
            }
            else if(itemTarget->category()==typePlugin)
            {// only plugin if target is plugin
                if(itemPaste->category()==typePlugin)
                    insertItem(index.parent(),itemPaste,itemTarget->row()+1);
            }
    }
    endMacro();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QML : get item at index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::setSelection(QModelIndexList aSelection,QItemSelection  aSelected,QItemSelection  aDeselected)
{
    // manage selection
    mSelectedIndices.clear();
    for(int i=0;i<aSelection.count();i++)
    {
        mSelectedIndices.append(QPersistentModelIndex(aSelection[i]));
    }
    // highlight selected
    QModelIndexList selected=aSelected.indexes();
    for(int i=0;i<selected.count();i++)
    {
        setData(selected[i],true,RoleSelected);
    }
    // unhighlight deselected
    QModelIndexList deselected=aDeselected.indexes();
    for(int i=0;i<deselected.count();i++)
    {
        setData(deselected[i],false,RoleSelected);
    }
    emit canPasteChanged();
    emit canCutOrCopyChanged();
}
//////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~ Helper functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ : get item at index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        QPersistentModelIndex pmi(index);
        TreeItem *item = static_cast<TreeItem*>(pmi.internalPointer());
        if (item) return item;
    }
    return rootItem;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ : get index by name
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QModelIndex TreeModel::getIndexByName(QString aName) const
{
    QModelIndexList Items = match(
                index(0, 0),
                RoleName,
                QVariant::fromValue(aName),
                2, // look *
                Qt::MatchRecursive); // look *
    if(!Items.isEmpty())
    {
        if(Items.size()>1) qDebug() << __FUNCTION__<< aName << "Match more than once";

        return Items[0];// must be unique
    }
    else{
        if(aName !="root") qDebug() << __FUNCTION__<< aName << "Not found";
        return QModelIndex();
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ : clone item at index
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem *TreeModel::cloneItem(const QModelIndex &index) const
{
    if (!index.isValid()) return nullptr;

    TreeItem *item = getItem(index);
    if(item->type()==typeConfig) return nullptr;      // not enabled
    return cloneItem(item);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// C++ : clone given item
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TreeItem *TreeModel::cloneItem(const TreeItem * aItem) const
{
    if(aItem==nullptr) return nullptr;
    if(aItem->type()==typeConfig) return nullptr;      // not enabled
    return new TreeItem(*aItem);    // copy constructor
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int TreeModel::setItemName(const QModelIndex &aIndex, const QVariant& aName)
{
    if (!aIndex.isValid()) return 1;

    if(aName.toString().isEmpty()) return 2;
    QModelIndex mi=getIndexByName(aName.toString());
    if(mi.isValid()) return 1;
    setData(aIndex,aName,RoleName);
    modified(true);

    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::setModuleType(const QModelIndex &aIndex,
                              const QVariant& aCategory,
                              const QVariant& aModuleType)
{
    if (!aIndex.isValid()) return;
    TreeItem *item = getItem(aIndex);
    setData(aIndex,aCategory,RoleCategory);
    setData(aIndex,aModuleType,RoleModuleType);
    modified(true);
    item->createItemParamList();
    emit dataChanged(aIndex, aIndex, {RoleParamList});
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ undo/redo wrapper ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::undo()
{
    if(mUndoManager.canUndo())
    {
        emit layoutAboutToBeChanged(); mUndoManager.undo(); emit layoutChanged();
    }
    modified(mUndoManager.canUndo());
    emit canUndoChanged();
    emit canRedoChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::redo()
{
    if(mUndoManager.canRedo())
    {
        emit layoutAboutToBeChanged(); mUndoManager.redo(); emit layoutChanged();
        modified(true);
    }
    emit canUndoChanged();
    emit canRedoChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::beginMacro()
{
    emit layoutAboutToBeChanged();
    mUndoManager.beginMacro();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::endMacro()
{
    mUndoManager.endMacro();
    emit layoutChanged();
    emit canUndoChanged();
    emit canRedoChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::canUndo()
{
    return (mUndoManager.canUndo());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::canRedo()
{
    return (mUndoManager.canRedo());
}
#ifdef TMP_NAME
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// get unique id for items
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string generate_hex(const unsigned int len) {
    std::stringstream ss;
    for (unsigned int i = 0; i < len; i++)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        std::stringstream hexstream;
        hexstream << std::hex << dis(gen);
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// check recursively if a name exists for a type
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::nameExist(QString aName,TreeItem* newItem, TreeItem* aLookupItem)
{
    if( aLookupItem->type()== newItem->type()
            && newItem != aLookupItem
            && aLookupItem->name()== aName) return true;

    //~~~~~~~ recursive check of module and plugin items ~~~~~~~~~~
    for (int i=0;i<aLookupItem->childCount();i++)
    {
        if(nameExist(aName,newItem,aLookupItem->child(i))) return true;
    }
    return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// set a unique name recursively
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TreeModel::setUniqueName(TreeItem* item)
{
    if(item->name().isEmpty())
    {
        qDebug() << __FUNCTION__<<"Empty name";
        return;
    }
    // set unique name for cloned items
#ifdef TMP_NAME
    item->name(item->type()+QString::fromStdString(generate_hex(2)));
#else
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Called after insertion of items due to copy/paste
    // Pb : when copy/paste btw 2 config item is already
    //      inserted and compared to itself so test is always true
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(item->type()!=typeFork && nameExist(item->name(),item,rootItem))
    {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Get unique name with suffix count
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        bool bExist;
        int suffix=1;
        QString vName;
        int pos = item->name().lastIndexOf(QChar('#'));
        QString radical=item->name().left(pos);
        //qDebug() << "\n~~~~~~~~~~~ " << __FUNCTION__<<"looking for " <<item->name();
        do
        {
            vName=QString("%1#%2").arg(radical).arg(suffix++);
            bExist=nameExist(vName,item,rootItem);
        }
        while(bExist);
        //qDebug() << __FUNCTION__<<"Changed "<< item->name() << "to "<<vName;

        item->name(vName);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
    for (int i=0;i<item->childCount();i++)
    {
        setUniqueName(item->child(i));
    }
}
//////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~ QAbstractItemModel overriden methods ~~~~~~~~~~~~~~~~~~
//////////////////////////////////////////////////////////////////////
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if(role==RoleSelected) return QVariant(item->isSelected());
    if(role==RoleHasError) return QVariant(item->hasError());
    if(role==RoleParamList) return QVariant::fromValue(item->getParamList());

    if (role - Qt::UserRole> columnCount(index)) return QVariant();
    else return item->data(role - Qt::UserRole-1);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())  return false;

    TreeItem *item = getItem(index);

    if(role==RoleSelected)
    {
        item->setSelected(value.toBool());
    }
    else if(role==RoleHasError)
    {
        item->hasError(value.toBool());
    }
    else
    {
        if (role-Qt::UserRole > columnCount(index))  return false;
        item->setData(role - Qt::UserRole-1, value);
    }
    emit dataChanged(index, index, {role});

    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem) return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QHash<int, QByteArray> TreeModel::roleNames() const
{
    return m_roleNameMapping;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// currently NOT USED
bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;
    QVector<QVariant> columnData;
    columnData << "xxx" << "xxx" << "";
    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, columnData);
    endInsertRows();

    emit layoutChanged();
    return success;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();
    return success;
}
