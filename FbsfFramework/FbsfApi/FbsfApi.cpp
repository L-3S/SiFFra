#include "FbsfApi.h"
#include "FbsfNetServer.h"
#include "FbsfExecutive.h"
#include <QtCore/QList>
#include <iostream>
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
    if (!comp || !comp->app) {
        qFatal("Error: no instance");
    } else if (!comp->app) {
        qFatal("Error: no app");
    } else if (comp->app->configName() == "") {
        qWarning("Error: no configuration provided, use fmi2SetString first.");
    } else {
        std::cout << "dazfaezpk3"<<std::endl;
        comp->app->setup(comp->app->configName());
        std::cout << "dazfaezpk4"<<std::endl;
        if(comp->app->config().parseXML(comp->app->configName())==-1) qWarning("Error: can't parse xml configuration file provided: ", comp->app->configName().toStdString().c_str());
        std::cout << "dazfaezpk5"<<std::endl;
        comp->app->generateSequences();
        std::cout << "dazfaezpk6"<<std::endl;
    }
    comp->app->start();
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
    qDebug() << "TERMINATE";
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
