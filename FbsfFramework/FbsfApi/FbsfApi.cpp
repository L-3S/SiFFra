#include "FbsfApi.h"
#include "FbsfNetServer.h"
#include "FbsfExecutive.h"
#include <QtCore/QList>
#include <iostream>
#include "FbsfApplication.h"
#include "FbsfConfiguration.h"

FbsfFmi2Component::FbsfFmi2Component(int aac, char **aav): ac(aac), av(aav), app(nullptr), run(false), threadRunning(false) {};

FbsfApi::FbsfApi()
{

}

fmi2Component FbsfApi::fmi2Instanciate(int argc, char **argv) {
    return new FbsfFmi2Component(argc, argv);
};

fmi2Status FbsfApi::fmi2EnterInitialisationMode(fmi2Component ptr) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp) {
        qInfo("Error: no instance");
        return fmi2Error;
    } else if (comp->configFileName == "") {
        qWarning("Error: no configuration provided, use fmi2SetString first.");
        return fmi2Error;
    }
    if (comp->run) {
        qInfo("Error: App is already runnning");
        return fmi2Error;
    }
    if (comp->threadRunning) {
        qInfo("Error: Initialisation mode already on");
        return fmi2Error;
    }
    comp->threadRunning = true;
//    fct(comp);
    comp->qtThread = std::thread(mainLoop, comp);
    return fmi2OK;
};

void FbsfApi::mainLoop(FbsfFmi2Component *comp) {
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
        qWarning("Error: no configuration provided, use fmi2SetString first.");
    } else {
        // Setup the app
        comp->app->setup();
        // Open and parse XML config
        if(comp->app->config().parseXML(comp->app->configName())==-1) qWarning("Error: can't parse xml configuration file provided: ", comp->app->configName().toStdString().c_str());
        // Generate sequences from XML infos
        comp->app->generateSequences();
    }
    // Wait for the fmi2ExitInitialisationMode function to be called to start app run
    while (1) {
        comp->mu.lock();
        if (comp->run == true) {
            comp->mu.unlock();
            break;
        }
        comp->mu.unlock();
    }
    // Launch the qt app loop
    comp->app->start();
    comp->threadRunning = false;
}
fmi2Status FbsfApi::fmi2ExitInitialisationMode(fmi2Component ptr){
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    if (!comp->threadRunning) {
        qInfo("Error: Initialisation mode is not  on");
        return fmi2Error;
    }
    // Set run variable to unlock FbsfApi::mainLoop and launch the app
    comp->mu.lock();
    comp->run = true;
    comp->mu.unlock();
    return fmi2OK;
};

fmi2Status FbsfApi::fmi2DoStep(fmi2Component ptr) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    if (!comp->run) {
        qInfo("Error: App in not running, please exit initialisation mode");
        return fmi2Error;
    }
    if (!comp->threadRunning) {
        qInfo("Error: QT thread is not running, please enter initialisation mode");
        return fmi2Error;
    }
    QString s = comp->app->executive()->State();
    if (s == "runnin" || s == "stepping") {
        qInfo("Error: A step is already running");
        return (fmi2Error);
    }
    comp->app->executive()->control("step");
    return fmi2Pending;
};

fmi2Status FbsfApi::fmi2CancelStep(fmi2Component ptr) {
      qInfo("Error: not implemented");
//    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);
//    if (!comp || !comp->app) {
//        return fmi2Error;
//    } else {
//        comp->app->executive()->control("cancel");
//    }
    return fmi2OK;
};

fmi2Status FbsfApi::fmi2Terminate(fmi2Component ptr) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    if (!comp->threadRunning || !comp->run) {
        qInfo("Error: The app is not running");
        return fmi2Error;
    }
//    QCoreApplication::exit(0);
    comp->app->executive()->control("stop");
//    comp->app->executive()->stopApp();
//    comp->qtThread.;
    if (comp->qtThread.joinable()) {
        comp->qtThread.join();
    }
    return fmi2OK;
};

fmi2Status FbsfApi::fmi2FreeInstance(fmi2Component ptr) {
    if (mBuff) {
        free(mBuff);
    }
    if (ptr) {
        FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

        delete comp;
        comp = nullptr;
    }
    return fmi2OK;
};
fmi2Status FbsfApi::fmi2GetStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Status *value)  {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    switch (int(s)) {
    case fmi2DoStepStatus: {
        QString status = comp->app->executive()->State();
        if (status == "stepping" || status == "runnning" || status == "waiting") {
            *value = fmi2Pending;
        } else {
            int exSt = comp->app->executive()->getStatus();
            switch (exSt) {
            case FBSF_OK:       *value = fmi2OK;
            case FBSF_WARNING:  *value = fmi2Warning;
            case FBSF_FATAL:    *value = fmi2Fatal;
            case FBSF_ERROR:    *value = fmi2Error;
            default:            *value = fmi2Error;
            }
        }
        break;
    }
    case fmi2PendingStatus: {break;}
    case fmi2LastSuccessfulTime: {break;}
    case fmi2Terminated: {break;}
    }
    return fmi2OK;
};
fmi2Status FbsfApi::fmi2GetRealStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Real *value) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    *value = comp->app->executive()->getLastStepSuccessTime();
    return fmi2OK;
};
fmi2Status FbsfApi::fmi2GetIntegerStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Integer *value) {
    return fmi2OK;
};
fmi2Status FbsfApi::fmi2GetBooleanStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Boolean *value) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    switch (int(s)) {
        case fmi2DoStepStatus: {break;}
        case fmi2PendingStatus: {break;}
        case fmi2LastSuccessfulTime: {break;}
        case fmi2Terminated: {
            QString status = comp->app->executive()->State();
            if (status == "stepping" || status == "runnning" || status == "waiting") {
                *value = fmi2False;
            } else {
                int exSt = comp->app->executive()->getStatus();
                cout << "exts" << exSt<< endl;
                switch (exSt) {
                case FBSF_OK:       *value = fmi2False;break;
                case FBSF_WARNING:  *value = fmi2False;break;
                case FBSF_FATAL:    *value = fmi2True;break;
                case FBSF_ERROR:    *value = fmi2False;break;
                default:            *value = fmi2False;break;
                }
            }
            break;
        }
        break;
    }

    return fmi2OK;
};
fmi2Status FbsfApi::fmi2GetStringStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2String *value) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    if (!value) {
        qInfo("Error: argument value is not allocated");
        return fmi2Error;
    }
    switch (int(s)) {
    case fmi2DoStepStatus: {
        break;
    }
    case fmi2PendingStatus: {
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
    case fmi2LastSuccessfulTime: {break;}
    case fmi2Terminated: {break;}
    }

    return fmi2OK;
};

fmi2Status FbsfApi::fmi2SetString(fmi2Component ptr, QString str) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp) {
        qInfo("Error: no instance");
        return fmi2Error;
    } else {
        comp->mu.lock();
        comp->configFileName = str;
        comp->mu.unlock();
    }
    return fmi2OK;
};
