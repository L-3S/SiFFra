#include "Controller.h"
#include "TreeItem.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDirIterator>
#include <QtGlobal>
#include <QMessageBox>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Controller::Controller()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString Controller::getAppHome()
{
    QString applicationHome = qgetenv("APP_HOME");
    if (applicationHome.isEmpty())
    {
        qDebug()<<__FUNCTION__ << "Environment variable APP_HOME not set";
        return QDir::currentPath();
    }
    return applicationHome;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// get relative path to APP_HOME
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString Controller::getRelativePath(QUrl aPath)
{
    QString applicationHome = qgetenv("APP_HOME");
    if (applicationHome.isEmpty())
    {
        QDir appHome(QDir::currentPath());
        return appHome.relativeFilePath(aPath.toLocalFile());
    }
    else
    {
        QDir appHome(applicationHome);
        return appHome.relativeFilePath(aPath.toLocalFile());
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::findModuleLib()
{
    QStringList filter;
#if (defined (_WIN32) || defined (_WIN64))
    filter << "*Module*.dll";
#elif (defined (LINUX) || defined (__linux__))
    filter << "libModule*.so";
#endif

    // first get the FBSF module libraries
    QDirIterator it(QCoreApplication::applicationDirPath(),
                    filter,QDir::AllEntries | QDir::NoDotAndDotDot);

    while (it.hasNext())
    {
        QFileInfo file=it.next();
        QString typeCpp=file.baseName();
        QString category=moduleCategoryMap[typeCpp];
        mModuleTypeList.append(new ModuleDescriptor(category,typeCpp,file.absoluteFilePath()));
    }
    // then get the application module libraries
    QString applicationHome = qgetenv("APP_HOME");
    if (!applicationHome.isEmpty())
    {
        QDirIterator it(applicationHome+"/lib",
                        filter,QDir::AllEntries | QDir::NoDotAndDotDot);
        while (it.hasNext())
        {
            QFileInfo file=it.next();
            QString typeCpp=file.baseName();
            QString category=moduleCategoryMap.value(typeCpp,"manual");
            mModuleTypeList.append(new ModuleDescriptor(category,typeCpp,file.absoluteFilePath()));
        }
    }
    ////////// Test purpose /////////
    mModuleTypeList.append(new ModuleDescriptor("fmu","fmu",""));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QAbstractItemModel* Controller::config()
{
    return dynamic_cast<QAbstractItemModel*>(mCurrentModel);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QAbstractItemModel* Controller::config1()
{
    return dynamic_cast<QAbstractItemModel*>(&mTreeModel1);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QAbstractItemModel* Controller::config2()
{
    return dynamic_cast<QAbstractItemModel*>(&mTreeModel2);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::setCurrentConfig(int aNum)
{
    mCurrentModel=(aNum==1?&mTreeModel1:&mTreeModel2);
    emit configChanged();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QUrl Controller::newConfig()
{
    QString defaultName="NewConfig";
    getConfig().modified(true);
    return getConfig().newModelData(defaultName);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::openConfig(QUrl aFilename)
{
    if((mCurrentModel==&mTreeModel2 && mTreeModel1.configUrl()==aFilename)
    || (mCurrentModel==&mTreeModel1 && mTreeModel2.configUrl()==aFilename))
    {
        QMessageBox msgBox;
        msgBox.setText("Opening twice same document is not allowed");
        msgBox.exec();
    }
    else
    {
        getConfig().setupModelData(aFilename.toLocalFile());
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::saveConfig()
{
    QString xmlConfig;
    getConfig().getXmlConfig(xmlConfig);
    QFile file(getConfig().configUrl().toLocalFile());

    if(!file.open(QIODevice::WriteOnly))
    {
        file.close();
        qDebug() << __FUNCTION__<< "failed to save" << file.fileName();
    } else {
        QTextStream out(&file);
        out << xmlConfig;
        file.close();
        getConfig().modified(false);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::saveConfigAs(QUrl aFilename)
{
    // update the config item with saved name
    getConfig().setConfigUrl(aFilename);
    saveConfig();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::closeConfig()
{
    getConfig().clearModelData();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const QString Controller::checkConfig()
{
    return getConfig().checkModelData();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::insertModule(const QModelIndex &index)
{
    getConfig().insertModule(index);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::forkItem(const QModelIndex &index)
{
    getConfig().forkItem(index);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::addPluginList()
{
    getConfig().addPluginList();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::moveItem(const QModelIndex &index, int dir)
{
    getConfig().moveItem(index,dir);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::removeSelection()
{
    getConfig().removeSelection();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::cutSelection()
{
    getConfig().cutSelection();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::copySelection()
{
    getConfig().copySelection();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::pasteSelection(const QModelIndex &index)
{
    getConfig().pasteSelection(index);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::setSelection(QModelIndexList aSelection, QItemSelection  aSelected, QItemSelection  aDeselected)
{
    getConfig().setSelection(aSelection,aSelected,aDeselected);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Controller::setItemName(const QModelIndex &aIndex,const QVariant& aName)
{
    return getConfig().setItemName(aIndex, aName);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::setModuleType(const QModelIndex &aIndex,
                               const QVariant& aModuleType)
{
    ModuleDescriptor* mod=getModuleDescriptor(aModuleType.toString());
    if(mod!=nullptr)
        getConfig().setModuleType(aIndex, mod->category(), aModuleType);
}
