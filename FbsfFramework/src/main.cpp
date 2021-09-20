#include "FbsfApplication.h"
#include "FbsfConfiguration.h"
#include "LicenseManager.h"

#include <QDebug>
#include <QObject>
#include <iostream>
#include <fstream>
#include <QScopedPointer>
#include <QtGlobal>
#include <QMutex>
#include <QDir>
#include <QResource>

#ifndef MODE_BATCH
#include <QtWidgets/QMessageBox>
#endif

void MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fmu Based Simulation Framework Application
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char **argv)
{
    QScopedPointer<FbsfApplication> app(FbsfApplication::app(argc, argv));

    qInstallMessageHandler(MessageHandler);
    #ifdef QT_DEBUG
        qSetMessagePattern("[%{time} %{type} %{file}:%{line}] %{message}");
    #else
        qSetMessagePattern("[%{type}] %{message}");
    #endif
    std::ios_base::sync_with_stdio();
    try{
    const QStringList args = FbsfApplication::parser().positionalArguments();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check if the configuration file is available
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (args.isEmpty())
    {
        QString msg="Argument 'configuration file.xml' is missing.\n"
                  + FbsfApplication::parser().helpText();
        qCritical(msg.toStdString().c_str());
        return 1;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // capture sdtout and stderr from C libraries
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (!FbsfApplication::parser().isSet(QCommandLineOption("no-logfile")))
    {
        QFileInfo fi(args[0]);
        QString Clogfile = fi.baseName()+".log"; //TODO PATH CONFIGURATION
        freopen (Clogfile.toStdString().c_str(),"w",stdout);
        freopen (Clogfile.toStdString().c_str(),"a",stderr);
    }
    else
    {
        qInstallMessageHandler(0);// restore default handler
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check application licensing
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LicenseManager lm(FbsfApplication::sFrameworkHome+"FbsfFramework.lic");
    if (!lm.check())
    {
        QString msg("Invalid license file, Please contact your software provider");
#ifndef MODE_BATCH
        QMessageBox::critical( nullptr, "[Fatal]", msg.toStdString().c_str());
#endif
        qFatal(msg.toStdString().c_str());
    }
#ifndef MODE_BATCH
    //~~~~~~~~~~ init resource from embedded qrc ~~~~~~~~~~~~~~~~~~
    Q_INIT_RESOURCE(GraphicEditor);
#endif
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Parse the command line arguments
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(app->config().parseXML(args[0])==-1) return -1;

    // check modeMcp
    bool modeMcp=app->config().Simulation().value("simuMpc")=="true"?true:false;
    app->setTimeDepend(modeMcp);

    // check option perfMeter
    bool optPerfMeter=app->config().Simulation().value("perfMeter")=="true"?true:false;
    app->setOptPerfMeter(optPerfMeter);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // creation of the QML viewer UI
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    app->loadQML();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get the simulation parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    float   timeStep    = 0.5;
    float   speedFactor = 1.0;// n > 1  => slow time by n
    uint    recorderSize = UINT_MAX;

    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qInfo() << "Framework version : " << FBSF_VERSION;
    qInfo() << "Current working path:" << QDir::currentPath();
    qInfo() << "Configuration file :" << args[0];

    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qInfo() << "Application path :" << app->sApplicationHome;
    qInfo() << "Components path :" <<  app->sComponentsPath;
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qInfo() << "Simulation parameters :";
    foreach ( const QString& key, app->config().Simulation().keys())
    {
        QString value=app->config().Simulation().value(key);
        qInfo() << "\t" << key << value;
        if (key == "timestep")      timeStep=value.toFloat();
        if (key == "speedfactor")   speedFactor=value.toFloat();
        if (key == "recorder")      recorderSize=value.toUInt();
    }
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // network client : should it be restricted to UI only ???
    if (app->mode()==client) return app->start(timeStep*1000,1,recorderSize);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get the sequences description
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QList<FbsfConfigSequence>::const_iterator iSeq;
    for (iSeq = app->config().Sequences().begin(); iSeq != app->config().Sequences().end(); ++iSeq)
    {
        FbsfConfigSequence sequence=*iSeq;
        QString sequenceName;

        float sequencePeriod=1;//default period to 1
        QMap<QString,QString> vMap = sequence.Descriptor();
        foreach ( const QString& key, vMap.keys())
        {
            QString value=vMap.value(key);

            if (key == "name")      sequenceName=value;
            if (key == "period")    sequencePeriod=value.toFloat();
        }
        app->addSequence(sequenceName,
                         sequencePeriod,
                         sequence.Nodes(),
                         sequence.Models(),
                         app.data());

    } // end Sequences
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    return app->start(timeStep*1000,speedFactor,recorderSize);

    }// end try
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Catch exceptions
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    catch (int e){
        qInfo() << "Exception: " << e << endl;
    }
    catch (char* e){
        qInfo() << "Exception : " << e << endl;
    }
    catch (...) {
        qInfo() << "Unknown exception ";
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Application message handler
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutex mutex;
    QMutexLocker lock(&mutex);
    static std::ofstream logFile("FbsfFramework.log");

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
