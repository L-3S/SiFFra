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

FbsfComponent FbsfApi::FbsfInstanciate(int argc, char **argv) {
    return new FbsfControllerComponent(argc, argv);
};

FbsfStatus FbsfApi::FbsfEnterInitialisationMode(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp) {
        qInfo("Error: no instance");
        return FbsfError;
    } else if (comp->configFileName == "") {
        qWarning("Error: no configuration provided, use FbsfSetString first.");
        return FbsfError;
    }
    if (comp->run) {
        qInfo("Error: App is already runnning");
        return FbsfError;
    }
    if (comp->threadRunning) {
        qInfo("Error: Initialisation mode already on");
        return FbsfError;
    }
    comp->threadRunning = true;
//    fct(comp);
    comp->qtThread = std::thread(mainLoop, comp);
    return FbsfOK;
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
        if(comp->app->config().parseXML(comp->app->configName())==-1) qWarning("Error: can't parse xml configuration file provided: ", comp->app->configName().toStdString().c_str());
        // Generate sequences from XML infos
        comp->app->generateSequences();
    }
    // Wait for the FbsfExitInitialisationMode function to be called to start app run
    while (1) {
        comp->mu.lock();
        if (comp->run == true) {
            comp->mu.unlock();
            break;
        }
        comp->mu.unlock();
    }
    // Launch the qt app loop
    comp->app->start(comp->app->timeStep * 1000, comp->app->speedFactor, comp->app->recorderSize);
    comp->threadRunning = false;
}
FbsfStatus FbsfApi::FbsfExitInitialisationMode(FbsfComponent ptr){
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return FbsfError;
    }
    if (!comp->threadRunning) {
        qInfo("Error: Initialisation mode is not  on");
        return FbsfError;
    }
    // Set run variable to unlock FbsfApi::mainLoop and launch the app
    comp->mu.lock();
    comp->run = true;
    comp->mu.unlock();
    return FbsfOK;
};

FbsfStatus FbsfApi::FbsfDoStep(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return FbsfError;
    }
    if (!comp->run) {
        qInfo("Error: App in not running, please exit initialisation mode");
        return FbsfError;
    }
    if (!comp->threadRunning) {
        qInfo("Error: QT thread is not running, please enter initialisation mode");
        return FbsfError;
    }
    QString s = comp->app->executive()->State();
    if (s == "runnin" || s == "stepping") {
        qInfo("Error: A step is already running");
        return (FbsfError);
    }
    comp->app->executive()->control("step");
    FbsfStatus st;
    FbsfGetStatus(ptr, FbsfDoStepStatus,  &st);
    while (st == FbsfPending) {
        FbsfGetStatus(ptr, FbsfDoStepStatus,  &st);
    }
    return FbsfOK;
};

FbsfStatus FbsfApi::FbsfCancelStep(FbsfComponent ptr) {
      qInfo("Error: not implemented");
//    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
//    if (!comp || !comp->app) {
//        return FbsfError;
//    } else {
//        comp->app->executive()->control("cancel");
//    }
    return FbsfOK;
};

FbsfStatus FbsfApi::FbsfTerminate(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return FbsfError;
    }
    if (!comp->threadRunning || !comp->run) {
        qInfo("Error: The app is not running");
        return FbsfError;
    }
//    QCoreApplication::exit(0);
    comp->app->executive()->control("stop");
//    comp->app->executive()->stopApp();
//    comp->qtThread.;
    if (comp->qtThread.joinable()) {
        comp->qtThread.join();
    }
    return FbsfOK;
};

FbsfStatus FbsfApi::FbsfFreeInstance(FbsfComponent ptr) {
    if (mBuff) {
        free(mBuff);
    }
    if (ptr) {
        FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

        delete comp;
        comp = nullptr;
    }
    return FbsfOK;
};
FbsfStatus FbsfApi::FbsfGetStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfStatus *value)  {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return FbsfError;
    }
    switch (int(s)) {
    case FbsfDoStepStatus: {
        QString status = comp->app->executive()->State();
        if (status == "stepping" || status == "runnning" || status == "waiting") {
            *value = FbsfPending;
        } else {
            int exSt = comp->app->executive()->getStatus();
            switch (exSt) {
            case FBSF_OK:       *value = FbsfOK;
            case FBSF_WARNING:  *value = FbsfWarning;
            case FBSF_FATAL:    *value = FbsfFatal;
            case FBSF_ERROR:    *value = FbsfError;
            default:            *value = FbsfError;
            }
        }
        break;
    }
    case FbsfPendingStatus: {break;}
    case FbsfLastSuccessfulTime: {break;}
    case FbsfTerminated: {break;}
    }
    return FbsfOK;
};
FbsfStatus FbsfApi::FbsfGetRealStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfReal *value) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return FbsfError;
    }
    *value = comp->app->executive()->getLastStepSuccessTime();
    return FbsfOK;
};
FbsfStatus FbsfApi::FbsfGetIntegerStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfInteger *value) {
    return FbsfOK;
};
FbsfStatus FbsfApi::FbsfGetBooleanStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfBoolean *value) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return FbsfError;
    }
    switch (int(s)) {
        case FbsfDoStepStatus: {break;}
        case FbsfPendingStatus: {break;}
        case FbsfLastSuccessfulTime: {break;}
        case FbsfTerminated: {
            QString status = comp->app->executive()->State();
            if (status == "stepping" || status == "runnning" || status == "waiting") {
                *value = FbsfFalse;
            } else {
                int exSt = comp->app->executive()->getStatus();
                cout << "exts" << exSt<< endl;
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

    return FbsfOK;
};
FbsfStatus FbsfApi::FbsfGetStringStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfString *value) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return FbsfError;
    }
    if (!value) {
        qInfo("Error: argument value is not allocated");
        return FbsfError;
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
    case FbsfTerminated: {break;}
    }

    return FbsfOK;
};

FbsfStatus FbsfApi::FbsfSetString(FbsfComponent ptr, QString str) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp) {
        qInfo("Error: no instance");
        return FbsfError;
    } else {
        comp->mu.lock();
        comp->configFileName = str;
        comp->mu.unlock();
    }
    return FbsfOK;
};
