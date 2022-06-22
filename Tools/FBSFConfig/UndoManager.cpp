#include "UndoManager.h"
#include "TreeModel.h"
#include "TreeItem.h"
//#define TRACE
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UndoManager::UndoManager()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UndoManager::record(QUndoCommand* aCmd)
{
    mUndoStack.push(aCmd);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UndoManager::undo()
{
    mUndoStack.undo();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UndoManager::redo()
{
    mUndoStack.redo();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int UndoManager::count()
{
    return mUndoStack.count();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ removeItemCommand ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
removeItemCommand::removeItemCommand(TreeModel &aModel, QModelIndex aIndex)
    : mModel(aModel),mPosition(aIndex.row())
{
    // We store the parent name in order to get the real index
    mParentName=mModel.getItem(aIndex.parent())->name();
    mItem=mModel.getItem(aIndex);
#ifdef TRACE
        qDebug() << this <<__FUNCTION__ << "clone Item"
                 << mParentName<<mItem->name()<< mPosition ;
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void removeItemCommand::undo()
{
    if(mItem->type()==typePluginList) mModel.hasPluginList(true);
    // re-insert item
    QModelIndex parentIndex=mModel.getIndexByName(mParentName);
    TreeItem* parentItem=mModel.getItem(parentIndex);
    parentItem->insertChild(mItem,mPosition);

    //FIX: get a copy by item in case removed
    mItem = mModel.cloneItem(mItem);

#ifdef TRACE
    qDebug() << this <<__FUNCTION__
            << mItem->parentItem()->name()<<mItem->name()
            << mPosition;
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void removeItemCommand::redo()
{
    if(mItem==nullptr) return;

    if(mItem->type()==typePluginList) mModel.hasPluginList(false);

    TreeItem* itemCopy = mModel.cloneItem(mItem); // get a copy
    // remove mItem
    QModelIndex parentIndex=mModel.getIndexByName(mParentName);
    mModel.removeRow(mPosition, parentIndex);
    // store the copy
    mItem=itemCopy;

    mModel.modified(true);
#ifdef TRACE
    qDebug() << this<<__FUNCTION__
            << parentIndex<<mParentName<<mItem->parentItem()<< mItem->name()
            << "at" << mPosition;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ insertItemCommand ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
insertItemCommand::insertItemCommand(TreeModel& aModel,const QModelIndex aParentIndex,
                                     TreeItem* aItem,int aPosition)
    : mModel(aModel),mItem(aItem),mPosition(aPosition)
{
    // We store the parent name in order to get the real index later
    mParentName=mModel.getItem(aParentIndex)->name();

#ifdef TRACE
    qDebug() << this << __FUNCTION__
             << "stored parent" << mParentName
             << "child item : "<< mItem->name();
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void insertItemCommand::undo()
{
    QModelIndex parentIndex=mModel.getIndexByName(mParentName);
    if(!parentIndex.isValid())
    {
        qDebug() << __FUNCTION__<< mParentName << " parent not found";
        return;
    }
    TreeItem* itemCopy = mModel.cloneItem(mItem); // get a copy

#ifdef TRACE
    qDebug() << this << __FUNCTION__
             << parentIndex <<mParentName
             << "stored item" << mItem->name() << "at" <<mPosition;
#endif

    if(mItem->type()==typePluginList) mModel.hasPluginList(false);
    // remove mItem
    mModel.removeRow(mPosition,parentIndex);
    // store the copy
    mItem=itemCopy;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void insertItemCommand::redo()
{
    // we get the model index by name since the parent is a new item
    QModelIndex parentIndex=mModel.getIndexByName(mParentName);

    if(!parentIndex.isValid())
    {
        qDebug() << __FUNCTION__<< mParentName << " parent not found";
        return;
    }
    TreeItem* parent=mModel.getItem(parentIndex);
    if(parent==nullptr )
    {
        qDebug() <<__FUNCTION__<< "null parent at index"<<parentIndex;
        return;
    }

    if(mItem->type()==typePluginList) mModel.hasPluginList(true);
    // Insert item
    parent->insertChild(mItem,mPosition);

    // We get unique name for Item(s)
    //      1. since Item(s) are inserted in treeModel
    //      2. Before we store a copy
    mModel.setUniqueName(mItem);

    //FIX: get a copy by item in case removed with undo
    mItem = mModel.cloneItem(mItem);

    mModel.modified(true);
#ifdef TRACE
    qDebug() << this << __FUNCTION__
             << parentIndex << parent->name()
             << "stored item" << mItem->name()<< "at" <<mPosition;
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~ moveItemCommand ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
moveItemCommand::moveItemCommand(TreeModel &aModel, QModelIndex aIndex, int aDest)
    : mModel(aModel),mPosition(aIndex.row()),mDestination(aDest)
{
    // We store the parent name in order to get the real index later
    mParentName=mModel.getItem(aIndex.parent())->name();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void moveItemCommand::undo()
{
    // we get the model index by name since the parent could be a new item
    QModelIndex parentIndex=mModel.getIndexByName(mParentName);
    if(!parentIndex.isValid())
    {
        qDebug() << __FUNCTION__<< mParentName << " parent not found";
        return;
    }
    TreeItem *parent = mModel.getItem(parentIndex);
    if(parent==nullptr)
    {
        qDebug() <<__FUNCTION__<< "null parent at index"<<parentIndex;
        return;
    }

    parent->moveChild(mDestination,mPosition);
#ifdef TRACE
    qDebug() <<__FUNCTION__<<"from"<< mDestination<< "to"<< mPosition;
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void moveItemCommand::redo()
{
    // we get the model index by name since the parent could be a new item
    QModelIndex parentIndex=mModel.getIndexByName(mParentName);
    if(!parentIndex.isValid())
    {
        qDebug() << __FUNCTION__<< mParentName << " parent not found";
        return;
    }
    TreeItem *parent = mModel.getItem(parentIndex);

    if(parent==nullptr)
    {
        qDebug() <<__FUNCTION__<< "null parent at index"<<parentIndex;
        return;
    }

    parent->moveChild(mPosition,mDestination);
    mModel.modified(true);

#ifdef TRACE
    qDebug() <<__FUNCTION__<<"from"<< mPosition<< "to"<< mDestination;
#endif
}
