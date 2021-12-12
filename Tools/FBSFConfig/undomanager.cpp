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
    : mModel(aModel),mIndex(aIndex), mParentIndex(mIndex.parent())
{
    mItem = mModel.cloneItem(mIndex); // get a copy by index
#ifdef TRACE
        qDebug() << this <<__FUNCTION__ << "clone Item"
                 <<mItem->name()<< mIndex ;
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void removeItemCommand::undo()
{
    if(mItem->type()==typePluginList) mModel.hasPluginList(true);
    // re-insert item
    mItem->parentItem()->insertChild(mItem,mIndex.row());
    //FIX: get a copy by item in case removed
    mItem = mModel.cloneItem(mItem);

#ifdef TRACE
    qDebug() << this <<__FUNCTION__
            << mItem->parentItem()->name()<<mItem->name()
            << mIndex.row();
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void removeItemCommand::redo()
{
    if(!mIndex.isValid()) return;

    if(mItem->type()==typePluginList) mModel.hasPluginList(false);

    TreeItem* itemCopy = mModel.cloneItem(mItem); // get a copy
    // remove mItem
    mModel.removeRow(mIndex.row(), mParentIndex);
    // store the copy
    mItem=itemCopy;

    mModel.modified(true);
#ifdef TRACE
    qDebug() << this<<__FUNCTION__
            << mItem->parentItem()->name()<<mItem->name()
            << mIndex.row();
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ insertItemCommand ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
insertItemCommand::insertItemCommand(TreeModel& aModel,const QModelIndex aParentIndex,
                                     TreeItem* aItem,int aPosition)
    : mModel(aModel),mParentIndex(aParentIndex),mItem(aItem),mPosition(aPosition)
{
    // We store the parent name in order to get the real index
    mParentName=mModel.getItem(mParentIndex)->name();

#ifdef TRACE
    qDebug() << this << __FUNCTION__
             << "stored parent name" << mParentName
             << "child item : "<< mItem;
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void insertItemCommand::undo()
{
#ifdef TRACE
    qDebug() << this << __FUNCTION__
             << mParentIndex << mModel.getItem(mParentIndex)->name()
             << "stored item" << mItem <<mPosition;
#endif
    TreeItem* itemCopy = mModel.cloneItem(mItem); // get a copy

    if(mItem->type()==typePluginList) mModel.hasPluginList(false);
    // remove mItem
    mModel.removeRow(mPosition,mParentIndex);
    // store the copy
    mItem=itemCopy;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void insertItemCommand::redo()
{
    // we get the model index by name since the parent is a new item
    mParentIndex=mModel.getIndexByName(mParentName);
    // doesn't work if add plugin
    if(mItem->type()!=typePluginList && !mParentIndex.isValid())
    {
        qDebug() << __FUNCTION__<< mParentName << " parent not found";
        return;
    }
    TreeItem* parent=mModel.getItem(mParentIndex);
    if(parent==nullptr )
    {
        qDebug() <<__FUNCTION__<< "null parent at index"<<mParentIndex;
        return;
    }

    if(mItem->type()==typePluginList) mModel.hasPluginList(true);
    // Insert item
    parent->insertChild(mItem,mPosition);
    //FIX: get a copy by item in case removed
    mItem = mModel.cloneItem(mItem);

    mModel.modified(true);
#ifdef TRACE
    qDebug() << this << __FUNCTION__
             << mParentIndex << parent->name()
             << "stored item" << mItem<<mPosition;
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~ moveItemCommand ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
moveItemCommand::moveItemCommand(TreeModel &aModel, QModelIndex aIndex, int aDest)
    : mModel(aModel),mIndex(aIndex),mDestination(aDest)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void moveItemCommand::undo()
{
    TreeItem *parent = mModel.getItem(mIndex.parent());
    if(parent==nullptr)
    {
        qDebug() <<__FUNCTION__<< "null parent at index"<<mIndex.parent();
    }
    else
    {
        parent->moveChild(mDestination,mIndex.row());
        //qDebug() <<__FUNCTION__<<mModel.getItem(mIndex)->name()<< mIndex.row();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void moveItemCommand::redo()
{
    TreeItem *parent = mModel.getItem(mIndex.parent());
    //        qDebug() <<__FUNCTION__<<mModel.getItem(mIndex)->name()
    //                <<"from"<< mIndex.row()<< "to"<< mDestination;
    parent->moveChild(mIndex.row(),mDestination);
    mModel.modified(true);
}
