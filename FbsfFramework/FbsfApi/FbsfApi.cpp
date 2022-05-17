#include "FbsfApi.h"
#include "FbsfNetServer.h"
#include "FbsfExecutive.h"
#include <QtCore/QList>
#include <iostream>
#include "FbsfApplication.h"
#include "FbsfConfiguration.h"

FbsfControllerComponent::FbsfControllerComponent(int aac, char **aav): ac(aac), av(aav), app(nullptr), run(false), threadRunning(false) {};

FbsfApi::FbsfApi()
{

}

FbsfComponent FbsfApi::FbsfInstantiate(QString str) {
    FbsfComponent comp = new FbsfControllerComponent(0, nullptr);
    FbsfSetString(comp, str);
    FbsfEnterInitialisationMode(comp);
    FbsfExitInitialisationMode(comp);
    return comp;
};

FbsfSuccess FbsfApi::FbsfEnterInitialisationMode(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp) {
        qInfo("Error: no instance");
        return StepFailure;
    } else if (comp->configFileName == "") {
        qWarning("Error: no configuration provided, use FbsfSetString first.");
        return StepFailure;
    }
    if (comp->run) {
        qInfo("Error: App is already runnning");
        return StepFailure;
    }
    if (comp->threadRunning) {
        qInfo("Error: Initialisation mode already on");
        return StepFailure;
    }
    comp->threadRunning = true;
//    fct(comp);
    comp->qtThread = std::thread(mainLoop, comp);
    return StepSuccess;
};

void FbsfApi::mainLoop(FbsfControllerComponent *comp) {
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
FbsfSuccess FbsfApi::FbsfExitInitialisationMode(FbsfComponent ptr){
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    if (!comp) {
        qInfo("Error: no instance");
        return StepFailure;
    }
    while (1) {
        comp->mu.lock();
        if (!comp->threadRunning) {
            qInfo("Error: Initialisation mode is not  on");
            return StepFailure;
        }
        if (comp->run == true) {
            comp->mu.unlock();
            break;
        }
        comp->mu.unlock();
    }
    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return StepFailure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: Initialisation mode is not  on");
        return StepFailure;
    }
    // Set run variable to unlock FbsfApi::mainLoop and launch the app
    return StepSuccess;
};

FbsfSuccess FbsfApi::FbsfDoStep(FbsfComponent ptr, int timeOut) {
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
    return StepSuccess;
};

FbsfSuccess FbsfApi::FbsfCancelStep(FbsfComponent ptr) {
      qInfo("Error: not implemented");
//    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
//    if (!comp || !comp->app) {
//        return stepFailure;
//    } else {
//        comp->app->executive()->control("cancel");
//    }
    return StepSuccess;
};

FbsfSuccess FbsfApi::FbsfTerminate(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return StepFailure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: The app is not running");
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

FbsfSuccess FbsfApi::FbsfFreeInstance(FbsfComponent *ptr) {
    if (mBuff) {
        free(mBuff);
    }
    if (*ptr) {
        FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(*ptr);
        if (comp)
            delete comp;
        *ptr = nullptr;
    }
    return StepSuccess;
};
FbsfSuccess FbsfApi::FbsfGetStatus(FbsfComponent ptr, FbsfStatus *value)  {
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
FbsfSuccess FbsfApi::FbsfGetRealStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfReal *value) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        *value = FbsfUninitialized;
        return StepFailure;
    }
    *value = comp->app->executive()->getLastStepSuccessTime();
    return StepSuccess;
};
FbsfSuccess FbsfApi::FbsfGetIntegerStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfInteger *value) {
    return StepSuccess;
};
FbsfSuccess FbsfApi::FbsfGetBooleanStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfBoolean *value) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        *value = FbsfUninitialized;
        return StepFailure;
    }
    switch (int(s)) {
        case FbsfDoStepStatus: {break;}
        case FbsfPendingStatus: {break;}
        case FbsfLastSuccessfulTime: {break;}
        case FbsfIsTerminated: {
            QString status = comp->app->executive()->State();
            if (status == "stepping" || status == "runnning" || status == "waiting") {
                *value = FbsfFalse;
            } else {
                int exSt = comp->app->executive()->getStatus();
                switch (exSt) {
                case FBSF_OK:       *value = FbsfFalse;break;
                case FBSF_WARNING:  *value = FbsfFalse;break;
                case FBSF_FATAL:    *value = FbsfTrue;break;
                case FBSF_ERROR:    *value = FbsfFalse;break;
                default:            *value = FbsfFalse;break;
                }
            }
            break;
        }
        break;
    }

    return StepSuccess;
};
FbsfSuccess FbsfApi::FbsfGetStringStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfString *value) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        copyStringToBuff("Fbsf component is uninitialized", value);
        return StepFailure;
    }
    if (!value) {
        qInfo("Error: argument value is not allocated");
        return StepFailure;
    }
    switch (int(s)) {
    case FbsfDoStepStatus: {
        break;
    }
    case FbsfPendingStatus: {
        QString status = comp->app->executive()->State();
        if (status == "stepping" || status == "runnning" || status == "waiting") {
            copyStringToBuff(status.toStdString(), value);
        } else {
            int exSt = comp->app->executive()->getStatus();
            switch (exSt) {
            case FBSF_OK:       copyStringToBuff("Step ended without issues", value); break;
            case FBSF_WARNING:  copyStringToBuff("Step ended with a warning", value); break;
            case FBSF_FATAL:    copyStringToBuff("Step ended with a fatal error", value); break;
            case FBSF_ERROR:    copyStringToBuff("Step ended with an error", value); break;
            default:            copyStringToBuff("Step ended with unrecognized status", value); break;
            }
        }
        break;
    }
    case FbsfLastSuccessfulTime: {break;}
    case FbsfIsTerminated: {break;}
    }

    return StepSuccess;
};

FbsfSuccess FbsfApi::FbsfSetString(FbsfComponent ptr, QString str) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp) {
        qInfo("Error: no instance");
        return StepFailure;
    } else {
        comp->mu.lock();
        comp->configFileName = str;
        comp->mu.unlock();
    }
    return StepSuccess;
};
FbsfSuccess FbsfApi::FbsfGetRealData(FbsfComponent ptr, QString name, double *val) {

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
