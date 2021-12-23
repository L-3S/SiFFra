#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ModuleDescriptor.h"
#include "FbsfConfiguration.h"
#include "TreeModel.h"
#include "UndoManager.h"

#include <QObject>
#include <QUrl>
#include <QDebug>
#include <QCommandLineParser>
class ModuleDescriptor;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Controller : public QObject
{
    Q_OBJECT
public:
    Controller();
    int             parseCommandLine(QStringList arglist);
    void            readModuleList(QString aFileName);
    void            findModuleLib(QString aPath); // search for Module libraries
    QCommandLineParser&     parser(){return mParser;}

    // QML interface
    Q_PROPERTY(QAbstractItemModel* config READ config NOTIFY configChanged)
    Q_PROPERTY(QAbstractItemModel* config1 READ config1 CONSTANT)
    Q_PROPERTY(QAbstractItemModel* config2 READ config2 CONSTANT)
    QAbstractItemModel* config();  // current tree model property accessor
    QAbstractItemModel* config1(); // tree model1 property accessor
    QAbstractItemModel* config2(); // tree model2 property accessor

    Q_INVOKABLE QString getAppHome();
    Q_INVOKABLE QString getRelativePath(QUrl aPath); // relative to APP_HOME EV
    Q_INVOKABLE void    setCurrentConfig(int aNum);  // current config #1 or #2

    // tree model configuration
    TreeModel&      getConfig() { return *mCurrentModel;}

    // QML interface
    Q_INVOKABLE QUrl newConfig();
    Q_INVOKABLE void openConfig(QUrl aFilename);
    Q_INVOKABLE void saveConfig();
    Q_INVOKABLE void saveConfigAs(QUrl aFilename);
    Q_INVOKABLE void closeConfig();
    Q_INVOKABLE const QString checkConfig();

    Q_INVOKABLE void addPluginList();

    Q_INVOKABLE void insertModule(const QModelIndex &index);
    Q_INVOKABLE void forkItem(const QModelIndex &index);

    Q_INVOKABLE void moveItem(const QModelIndex &index, int dir);
    Q_INVOKABLE void removeSelection();
    Q_INVOKABLE void setSelection(QModelIndexList aSelection, QItemSelection  aSelected, QItemSelection  aDeselected);
    Q_INVOKABLE void cutSelection();
    Q_INVOKABLE void copySelection();
    Q_INVOKABLE void pasteSelection(const QModelIndex &index);
    Q_INVOKABLE void undo() { mCurrentModel->undo();}
    Q_INVOKABLE void redo() { mCurrentModel->redo();}
    Q_INVOKABLE int setItemName(const QModelIndex &aIndex,const QVariant& aName);
    Q_INVOKABLE void setModuleType(const QModelIndex &aIndex,
                                   const QVariant &aModuleType);
    Q_INVOKABLE void checkParams(const QModelIndex &aIndex);

    Q_INVOKABLE void itemParamChanged(int aIndex,
                                      const QVariant& aName,
                                      const QVariant& aValue)
    {
        Q_UNUSED(aIndex) Q_UNUSED(aName) Q_UNUSED(aValue)
        mCurrentModel->modified(true);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Q_PROPERTY(QList<QObject*> moduleTypeList READ moduleTypeList NOTIFY moduleTypeListChanged)

    ModuleDescriptor* getModuleDescriptor(QString aName)
    {
        for(QObject* mod:mModuleTypeList)
        {
            if(dynamic_cast<ModuleDescriptor*>(mod)->typeCpp()==aName)
                return dynamic_cast<ModuleDescriptor*>(mod);
        }
        qDebug() <<__FUNCTION__<< aName<< "module type not found";
        return nullptr;
    }
    QList<QObject*> moduleTypeList() { return mModuleTypeList;}

private :
    TreeModel           mTreeModel1;
    TreeModel           mTreeModel2;
    QList<QObject*>     mModuleTypeList;
    TreeModel*          mCurrentModel=&mTreeModel1;
    QCommandLineParser  mParser;

signals :
    void configChanged();
    void moduleTypeListChanged();

};

#endif // CONTROLLER_H
