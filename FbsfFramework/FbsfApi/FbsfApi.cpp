#include "FbsfApi.h"
#include "FbsfNetServer.h"
#include "FbsfExecutive.h"
#include <QtCore/QList>
#include <iostream>
#include "FbsfApplication.h"
#include "FbsfConfiguration.h"

FbsfFmi2Component::FbsfFmi2Component(int aac, char **aav): ac(aac), av(aav) {};

FbsfApi::FbsfApi()
{

}


fmi2Component FbsfApi::instanciate(int argc, char **argv) {
    return new FbsfFmi2Component(argc, argv);
};

fmi2Status FbsfApi::fmi2EnterInitialisationMode(fmi2Component ptr) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    comp->qtThread = std::thread(fct, comp);
    return fmi2OK;
};

void FbsfApi::fct(FbsfFmi2Component *comp) {
    comp->app = static_cast<FbsfApplication*>(mainApi(comp->ac, comp->av));
    comp->app->config().Name() = comp->str;
    if (!comp || !comp->app) {
        qFatal("Error: no instance");
    } else if (!comp->app) {
        qFatal("Error: no app");
    } else if (comp->app->configName() == "") {
        qWarning("Error: no configuration provided, use fmi2SetString first.");
    } else {
        comp->app->setup(comp->app->configName());
        if(comp->app->config().parseXML(comp->app->configName())==-1) qWarning("Error: can't parse xml configuration file provided: ", comp->app->configName().toStdString().c_str());
        comp->app->generateSequences();
    }
    while (1) {
        comp->mu.lock();
        if (comp->run == true) {
            comp->mu.unlock();
            break;
        }
        comp->mu.unlock();
    }
    comp->app->start();
}
fmi2Status FbsfApi::fmi2ExitInitialisationMode(fmi2Component ptr){
    //start ?
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
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
    QString s = comp->app->executive()->State();
    if (s == "runnin" || s == "stepping") {
        return (fmi2Error);
    }
    comp->app->executive()->control("step");
    return fmi2Pending;
};

fmi2Status FbsfApi::fmi2CancelStep(fmi2Component ptr) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);
    if (!comp || !comp->app) {
        qFatal("Error: no instance");
        return fmi2Error;
    } else {
        comp->app->executive()->control("cancel");
    }
    return fmi2OK;
};

fmi2Status FbsfApi::fmi2Terminate(fmi2Component ptr) {
    qDebug() << "TERMINATE";
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    comp->app->executive()->stopApp();
    comp->qtThread.join();
    qDebug() << "TERMINATEd";
    return fmi2OK;
};

fmi2Status FbsfApi::fmi2FreeInstance(fmi2Component ptr) {return fmi2OK;};
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
            *value = fmi2OK;
        }
    }
    case fmi2PendingStatus: {}
    case fmi2LastSuccessfulTime: {}
    case fmi2Terminated: {}
    }
    return fmi2OK;
};
fmi2Status FbsfApi::fmi2GetRealStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Real *value) {
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
        case fmi2DoStepStatus: {}
        case fmi2PendingStatus: {}
        case fmi2LastSuccessfulTime: {}
        case fmi2Terminated: {
            QString status = comp->app->executive()->State();
            if (status == "paused" || status == "stopped") {
                *value = fmi2True;
            } else {
                *value = fmi2False;
            }
        }
    }

    return fmi2OK;
};
fmi2Status FbsfApi::fmi2GetStringStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2String *value) {
    FbsfFmi2Component *comp = static_cast<FbsfFmi2Component*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return fmi2Error;
    }
    switch (int(s)) {
    case fmi2DoStepStatus: {
    }
    case fmi2PendingStatus: {
        QString status = comp->app->executive()->State();
        std::cout << status.toStdString() << std::endl;
        copyStringToBuff(status.toStdString(), value);
        std::cout << "status.toStdString()" << std::endl;
    }
    case fmi2LastSuccessfulTime: {}
    case fmi2Terminated: {}
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
        comp->str = str;
        comp->mu.unlock();
    }
    return fmi2OK;
};
