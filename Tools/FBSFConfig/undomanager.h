#ifndef UNDOMANAGER_H
#define UNDOMANAGER_H
#include <QUndoStack>
#include <QUndoCommand>
#include <QModelIndex>

class TreeModel;
class TreeItem;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class UndoManager
{
public:
    UndoManager();
    void record(QUndoCommand* aCmd);
    void undo();
    void redo();
    int count();
    void beginMacro() {mUndoStack.beginMacro("macro");}
    void endMacro()   {mUndoStack.endMacro();}
    bool canUndo() {return mUndoStack.canUndo();}
    bool canRedo() {return mUndoStack.canRedo();}
private:
    QUndoStack mUndoStack;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class removeItemCommand : public QUndoCommand
{
public:
    removeItemCommand(TreeModel& aModel,QModelIndex aIndex);
    void undo() override;
    void redo() override;
private:
    TreeModel&      mModel;
    TreeItem*       mItem;
    QModelIndex     mIndex;
    QModelIndex     mParentIndex;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class moveItemCommand : public QUndoCommand
{
public:
    moveItemCommand(TreeModel& aModel, QModelIndex aIndex, int aDest);
    void undo() override;
    void redo() override;
private:
    TreeModel&      mModel;
    TreeItem*       mItem;
    QModelIndex     mIndex;
    int             mDestination;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class insertItemCommand : public QUndoCommand
{
public:
    insertItemCommand(TreeModel& aModel, const QModelIndex aParentIndex,
                      TreeItem* aItem, int aPosition);
    void undo() override;
    void redo() override;
private:
    TreeModel&      mModel;
    QModelIndex     mParentIndex;
    QString         mParentName;
    TreeItem*       mItem;
    int             mPosition;
};
#endif // UNDOMANAGER_H
