#include "UndoManager.h"
#include "TreeModel.h"
#include "TreeItem.h"

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
    //    qDebug() <<__FUNCTION__ << "cloneItem"
    //             <<mItem->name()<< mIndex ;

    mItem = mModel.cloneItem(mIndex); // get a copy by index
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void removeItemCommand::undo()
{
    if(mItem->type()==typePlugins) mModel.hasPluginList(true);

    mItem->parentItem()->insertChild(mItem,mIndex.row());
//    qDebug() <<__FUNCTION__
//            << mItem->parentItem()->name()<<mItem->name()
//            << mIndex.row();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void removeItemCommand::redo()
{
    TreeItem* itemCopy = mModel.cloneItem(mItem); // get a copy by item

    if(!mIndex.isValid()) return;

    if(mItem->type()==typePlugins) mModel.hasPluginList(false);

    mModel.removeRow(mIndex.row(), mParentIndex);

    mItem=itemCopy;
    mModel.modified(true);

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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ insertItemCommand ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
insertItemCommand::insertItemCommand(TreeModel& aModel,const QModelIndex aParentIndex,
                                     TreeItem* aItem,int aPosition)
    : mModel(aModel),mParentIndex(aParentIndex),mItem(aItem),mPosition(aPosition)
{
    // We store the parent name in order to get the real index
    mParentName=mModel.getItem(mParentIndex)->name();
    //qDebug() << __FUNCTION__<< "stored parent name" << mParentName;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void insertItemCommand::undo()
{
    TreeItem* itemCopy = mModel.cloneItem(mItem); // get a copy by item
//    qDebug() << __FUNCTION__<< mParentIndex << mModel.getItem(mParentIndex)->name()
//             << "stored item" << mItem->name()<<mPosition;
    if(mItem->type()==typePlugins) mModel.hasPluginList(false);

    mModel.removeRow(mPosition,mParentIndex);

    mItem=itemCopy;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void insertItemCommand::redo()
{
    // we get the model index by name since the parent is a new item
    mParentIndex=mModel.getIndexByName(mParentName);
    TreeItem* parent=mModel.getItem(mParentIndex);
    if(parent==nullptr )
    {
        qDebug() <<__FUNCTION__<< "null parent at index"<<mParentIndex;
    }
    else
    {
        if(mItem->type()==typePlugins) mModel.hasPluginList(true);
        //qDebug() << __FUNCTION__<<parent->name() <<mItem->name()<< mPosition;
        parent->insertChild(mItem,mPosition);
    }
    mModel.modified(true);
}
