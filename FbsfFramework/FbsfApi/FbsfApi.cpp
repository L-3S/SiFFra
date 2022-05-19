#include "FbsfApi.h"
#include "FbsfNetServer.h"
#include "FbsfExecutive.h"
#include <QtCore/QList>
#include <iostream>
#include "FbsfApplication.h"
#include "FbsfConfiguration.h"

FbsfControllerComponent::FbsfControllerComponent(int aac, char **aav): ac(aac), av(aav), app(nullptr), run(false), threadRunning(false) {};

//FbsfApi()
//{

//}

FbsfComponent FbsfInstantiate(QString str) {
    FbsfControllerComponent *comp = new FbsfControllerComponent(0, nullptr);
    if (!comp) {
        qInfo("Error: no instance");
        return comp;
    } else {
        comp->mu.lock();
        comp->configFileName = str;
        comp->mu.unlock();
    }
    if (comp->configFileName == "") {
        qWarning("Error: no configuration provided");
        return comp;
    }
    comp->threadRunning = true;
//    fct(comp);
    comp->qtThread = std::thread(mainLoop, comp);
    while (1) {
        comp->mu.lock();
        if (!comp->threadRunning) {
            qInfo("Error: Initialisation mode is not  on");
            comp->mu.unlock();
            break;
        }
        if (comp->run == true && comp->app->executive()->State() != "initialized") {
            qDebug() << "Antoine "<< comp->app->executive()->State();
            comp->mu.unlock();
            break;
        }
        comp->mu.unlock();
    }
    return comp;
};

void mainLoop(FbsfControllerComponent *comp) {
    // Create an app instance

    QScopedPointer<FbsfApplication> app(static_cast<FbsfApplication*>(mainApi(comp->ac, comp->av)));
    comp->app = app.data();
    // Retreive configuration name
    comp->app->config().Name() = comp->configFileName;
    if (!comp) {
        qFatal("Error: no instance");
    } else if (!comp->app) {
        qFatal("Error: no app");
    } else if (comp->app->configName() == "") {
        qWarning("Error: no configuration provided, use FbsfSetString first.");
    } else {
        // Open and parse XML config
        if(comp->app->config().parseXML(comp->app->configName())==-1) {
            qWarning("Error: can't parse xml configuration file provided: ", comp->app->configName().toStdString().c_str());
            comp->threadRunning = false;
            return;
        }
        // Generate sequences from XML infos
        comp->app->generateSequences();
    }
    // Wait for the FbsfExitInitialisationMode function to be called to start app run
    comp->mu.lock();
    comp->run = true;
    comp->mu.unlock();
    // Launch the qt app loop
    comp->app->start(comp->app->timeStep * 1000, comp->app->speedFactor, comp->app->recorderSize);
    comp->threadRunning = false;
}

FbsfSuccess FbsfDoStep(FbsfComponent ptr, int timeOut) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    FbsfStatus st;

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return StepFailure;
    }
    if (!comp->run) {
        qInfo("Error: App in not running, please exit initialisation mode");
        return StepFailure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: QT thread is not running, please enter initialisation mode");
        return StepFailure;
    }
    QString s = comp->app->executive()->State();
    if (s == "runnin" || s == "stepping") {
        qInfo("Error: A step is already running");
        return (StepFailure);
    }
    comp->app->executive()->control("step", QString::number(timeOut));
    FbsfGetStatus(ptr, &st);
    while (st == FbsfProcessing) {
        FbsfGetStatus(ptr,  &st);
    }
    if (st == FbsfReady)
        comp->app->executive()->control("saveLocal");
    return StepSuccess;
};

FbsfSuccess FbsfCancelStep(FbsfComponent ptr) {
      qInfo("Error: not implemented");
//    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
//    if (!comp || !comp->app) {
//        return stepFailure;
//    } else {
//        comp->app->executive()->control("cancel");
//    }
    return StepSuccess;
};

FbsfSuccess FbsfTerminate(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return StepFailure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: The app is not running");
        if (comp->qtThread.joinable()) {
            comp->qtThread.join();
        }
        return StepFailure;
    }
//    QCoreApplication::exit(0);
    comp->app->executive()->control("stop");
//    comp->app->executive()->stopApp();
//    comp->qtThread.;
    if (comp->qtThread.joinable()) {
        comp->qtThread.join();
    }
    comp->isTerminated = true;
    return StepSuccess;
};

FbsfSuccess FbsfFreeInstance(FbsfComponent *ptr) {
    if (ptr && *ptr) {
        FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(*ptr);
        if (comp)
            delete comp;
        *ptr = nullptr;
    }
    return StepSuccess;
};
FbsfSuccess FbsfGetStatus(FbsfComponent ptr, FbsfStatus *value)  {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        *value = FbsfUninitialized;
        return StepSuccess;
    }
    if (comp->configFileName == "") {
        *value = FbsfUninitialized;
        return StepSuccess;
    }
    if (comp->isTerminated) {
        *value = FbsfTerminated;
        return StepSuccess;
    }
    if (!comp->run) {
        qInfo("Error: App in not running, please exit initialisation mode");
        *value = FbsfUninitialized;
        return StepSuccess;
    }
    if (!comp->threadRunning) {
        qInfo("Error: QT thread is not running, please enter initialisation mode");
        *value = FbsfUninitialized;
        return StepSuccess;
    }

    QString status = comp->app->executive()->State();
    if (status == "stepping" || status == "runnning" || status == "waiting") {
        *value = FbsfProcessing;
        return StepSuccess;
    } else {
        int exSt = comp->app->executive()->getStatus();
        switch (exSt) {
        case FBSF_OK:       *value = FbsfReady;break;
        case FBSF_WARNING:  *value = FbsfFailedStep;break;
        case FBSF_FATAL:    *value = FbsfFailedStep;break;
        case FBSF_ERROR:    *value = FbsfFailedStep;break;
        case FBSF_TIMEOUT:  *value = FbsfTimeOut;break;
        default:            *value = FbsfFailedStep;
        }
        return StepSuccess;
    }
    *value = FbsfReady;
    return StepSuccess;
};
FbsfSuccess FbsfGetRealData(FbsfComponent ptr, QString name, double *val) {

    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->Class() == cScalar && data->name() == name) {
            QStringList description;
//            description.append(data->TypeToStr());
//            description.append(data->producer());
//            description.append(data->unit());
//            description.append(data->description());
            description.append(data->name());
            // Scalar or Vector
//            description.append(data->ClassToStr());
//            description.append(QString::number(data->size()));
            *val = data->value().toDouble();
            return StepSuccess;
        }
    }
    return StepFailure;
}

FbsfSuccess API_EXPORT FbsfResetData(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    comp->app->executive()->control("restoreLocal");
    return StepFailure;
}
