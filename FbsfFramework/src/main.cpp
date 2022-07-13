#include "FbsfApplication.h"
#include "FbsfConfiguration.h"

#include <QDebug>
#include <QObject>
#include <iostream>
#include <fstream>
#include <QScopedPointer>
#include <QtGlobal>
#include <QMutex>
#include <QDir>
#include <QResource>
#include <thread>
#ifndef MODE_BATCH
#include <QtWidgets/QMessageBox>
#endif

#ifdef BUILD_API
#include "FbsfApi.h"
#endif

void MessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Fmu Based Simulation Framework Application
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef BUILD_API
    int main(int argc, char **argv)
#else
void* mainApi(int argc, char **argv)
#endif
{
    FbsfDataExchange::sPublicDataMap.clear();
    qDebug() << "Antoine Map == " << FbsfDataExchange::sPublicDataMap.keys();
#ifndef MODE_BATCH
    //~~~~~~~~~~ init resource from embedded qrc ~~~~~~~~~~~~~~~~~~
    Q_INIT_RESOURCE(GraphicEditor);
#endif
//    qInstallMessageHandler(MessageHandler);
    #ifdef QT_DEBUG
        qSetMessagePattern("[%{time} %{type} %{file}:%{line}] %{message}");
    #else
        qSetMessagePattern("[%{type}] %{message}");
    #endif
    #ifdef BUILD_API
        return FbsfApplication::app(argc, argv);
    #endif
    std::ios_base::sync_with_stdio();
    QScopedPointer<FbsfApplication> app(FbsfApplication::app(argc, argv));
    try{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check if the configuration file is available
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    app->parseArguments();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check application licensing
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    #ifndef BUILD_API
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Parse the command line arguments
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(app->config().parseXML(app->configName())==-1) return -1;
    app->generateSequences();


    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    return app->start(app->timeStep*1000,app->speedFactor,app->recorderSize);
    #endif
    }// end try
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Catch exceptions
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    catch (int e){
        qInfo() << "Exception: " << e << Qt::endl;
    }
    catch (char* e){
        qInfo() << "Exception : " << e << Qt::endl;
    }
    catch (...) {
        qInfo() << "Unknown exception ";
    }
//    std::thread th(app->start(timeStep*1000,speedFactor,recorderSize));
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
