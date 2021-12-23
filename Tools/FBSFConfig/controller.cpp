#include "Controller.h"
#include "TreeItem.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDirIterator>
#include <QtGlobal>
#include <QMessageBox>
#include <QFile>
#include <QMutex>
#include <iostream>
#include <fstream>

void MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Controller::Controller()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Controller::parseCommandLine(QStringList arglist)
{
    mParser.setApplicationDescription(QCoreApplication::translate("main","Editor for simulation configuration"));
    mParser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    mParser.addHelpOption();
    mParser.addVersionOption();
    mParser.addPositionalArgument("Configuration file",
                                  QCoreApplication::translate("main", " : xml description file"));

    QCommandLineOption noLogfileOption(QStringList() << "n" << "no-logfile",
                                       QCoreApplication::translate("main", "Don't log to file."));
    mParser.addOption(noLogfileOption);// no logfile

    QCommandLineOption typeModuleList(QStringList() << "ml" << "module-list",
                                      "Read type of modules from <file>.", "file");
    mParser.addOption(typeModuleList);// type module

    mParser.process(arglist);

    const QStringList args = mParser.positionalArguments();

    QString mlFile  = mParser.value(typeModuleList);

    if (!mParser.isSet(QCommandLineOption("no-logfile")))
    {
        qInstallMessageHandler(MessageHandler);
#ifdef QT_DEBUG
        qSetMessagePattern("[%{time} %{type} %{file}:%{line}] %{message}");
#else
        qSetMessagePattern("[%{type}] %{message}");
#endif
    }
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qInfo() << "FBSF_HOME dir       : "<< qgetenv("FBSF_HOME");
    qInfo() << "APP_HOME dir        : "<< getAppHome();
    qInfo() << "Current working dir : "<< QDir::currentPath();
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    // get the description for Fbsf modules
    qInfo() << "FBSF Module list : ";
    findModuleLib(QCoreApplication::applicationDirPath());
    mModuleTypeList.append(new ModuleDescriptor("fmu","fmu",""));
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    if (!mlFile.isEmpty())
    {   // defined file
        qInfo() << "Project Module list : " << mlFile;
        readModuleList(mlFile);
    }
    else
    {   // default file
        mlFile=getAppHome()+"/"+"ModuleList.lst";
        if(QFile::exists(mlFile))
        {
            qDebug() << "Project Module list :"<< mlFile;
            readModuleList(mlFile);
        }
        else
        {
            qDebug() << "Looking in APP_HOME/lib for Modulexxx[.dll,.so] ";
            // Get the application module libraries
            findModuleLib(getAppHome()+"/lib");
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check if the configuration file is available
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (!args.isEmpty())
    {
        const QString filename = args.at(0);
        qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
        qInfo() << "Loading configuration file : " << filename;
        getConfig().setupModelData(filename);// load config
    }
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::readModuleList(QString aFileName)
{
    QFile vFile(aFileName);
    if (!vFile.open(QFile::ReadOnly))
    {
        qDebug() << "Error openning " << aFileName << vFile.errorString();
        return;
    }
    QString moduleType;
    QTextStream inStream( &vFile );
    inStream.setCodec("UTF-8");

    do {
        moduleType = inStream.readLine();
        if((!moduleType.isNull()))
        {
            mModuleTypeList.append(new ModuleDescriptor("manual",moduleType,moduleType+".dll"));
        }
    } while (!moduleType.isNull());
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
void Controller::findModuleLib(QString aPath)
{
    QStringList filter;
#if (defined (_WIN32) || defined (_WIN64))
    filter << "*Module*.dll";
#elif (defined (LINUX) || defined (__linux__))
    filter << "libModule*.so";
#endif

    // first get the FBSF module libraries
    QDirIterator it(aPath,filter,QDir::AllEntries | QDir::NoDotAndDotDot);

    while (it.hasNext())
    {
        QFileInfo file=it.next();
        QString typeCpp=file.baseName();
        QString category=moduleCategoryMap[typeCpp];
        mModuleTypeList.append(new ModuleDescriptor(category,typeCpp,file.fileName()));
    }
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Controller::checkParams(const QModelIndex &aIndex)
{
    getConfig().checkItemParams(aIndex);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Application message handler
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutex mutex;
    QMutexLocker lock(&mutex);
    static std::ofstream logFile("FbsfConfig.log");

    if (logFile)
    {
        logFile << qPrintable(qFormatLogMessage(type,context,msg)) << std::endl;
        if(type==QtFatalMsg)
        {
#ifndef MODE_BATCH
            QMessageBox::critical( nullptr, "[Fatal]", qPrintable(msg));
#endif
            abort();
        }
    }
}
