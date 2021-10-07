#include "FbsfApi.h"
#include "FbsfNetServer.h"
#include "FbsfExecutive.h"
#include <QtCore/QList>
#include "FbsfApplication.h"
#include "FbsfConfiguration.h"

Fmi2Component::Fmi2Component(int aac, char **aav): ac(aac), av(aav) {};

FbsfApi::FbsfApi()
{

}

void *FbsfApi::instanciate(int argc, char **argv) {
    return new Fmi2Component(argc, argv);
};

void FbsfApi::fmi2EnterInitialisationMode(void *ptr) {
    Fmi2Component *comp = static_cast<Fmi2Component*>(ptr);

};

void FbsfApi::fct(Fmi2Component *comp) {
    comp->app = static_cast<FbsfApplication*>(mainApi(comp->ac, comp->av));
    comp->app->config().Name() = comp->str;
    qDebug("start app");
    if (!comp || !comp->app) {
        qFatal("Error: no instance");
    } else if (comp->app->configName() == "") {
        qWarning("Error: no configuration provided, use fmi2SetString first.");
    } else {
        comp->app->setup(comp->app->configName());
        if(comp->app->config().parseXML(comp->app->configName())==-1) qWarning("Error: can't parse xml configuration file provided: ", comp->app->configName().toStdString().c_str());
        comp->app->generateSequences();
    }
    comp->app->start();
    qDebug("stop app");
}

void FbsfApi::fmi2ExitInitialisationMode(void *ptr){
    //start ?
    Fmi2Component *comp = static_cast<Fmi2Component*>(ptr);

//    fct(comp);
    comp->th = std::thread(fct, comp);
//    app->th;
};

void FbsfApi::fmi2DoStep(void *ptr) {
    Fmi2Component *comp = static_cast<Fmi2Component*>(ptr);
//    emit guiControl("step");
    comp->app->executive()->control("step");
};

void FbsfApi::fmi2CancelStep(void *ptr) {
    Fmi2Component *comp = static_cast<Fmi2Component*>(ptr);
    if (!comp || !comp->app) {
        qFatal("Error: no instance");
    } else {
        comp->app->executive()->control("cancel");
    }
};

void FbsfApi::fmi2Terminate(void *ptr) {
    Fmi2Component *comp = static_cast<Fmi2Component*>(ptr);
    comp->th.join();
};

void FbsfApi::fmi2FreeInstance(void *ptr) {};

void FbsfApi::fmi2GetStatus(void *ptr) {};

void FbsfApi::fmi2GetRealStatus(void *ptr) {};

void FbsfApi::fmi2GetIntegerStatus(void *ptr) {};

void FbsfApi::fmi2GetBooleanStatus(void *ptr) {};

void FbsfApi::fmi2GetStringStatus(void *ptr) {};

void FbsfApi::fmi2SetString(void *ptr, QString str) {
    Fmi2Component *comp = static_cast<Fmi2Component*>(ptr);

    if (!comp) {
        qFatal("Error: no instance");
    } else {
        comp->str = str;
    }
};
