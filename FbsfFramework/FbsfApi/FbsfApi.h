#ifndef FBSFAPI_H
#define FBSFAPI_H

#include <thread>
#include <QString>
#include <QObject>
#include <QThread>
#include <QDebug>

#if defined(BUILD_API)
#  define API_EXPORT Q_DECL_EXPORT
#else
#  define API_EXPORT Q_DECL_IMPORT
#endif

class FbsfApplication;
class API_EXPORT Fmi2Component: public QObject {
    Q_OBJECT
public:
    int ac;
    char **av;
    Fmi2Component(int, char **);
    FbsfApplication *app;
    std::thread th;
    QString str;
};

//class API_EXPORT Ctrlr : public QObject {
//    Q_OBJECT
//public:
//    Ctrlr() {
//    }
//    void handleResults(const QString &);
//    void runApp(Fmi2Component *Worker);
//signals:
//    void operate(QString &);
//};

//class API_EXPORT Container {
//public:
//    Container(Fmi2Component *cp): comp(cp) {
//        ctrl = new Ctrlr();
//    }
//    void runC() {
//        ctrl->runApp(comp);
//    }
//    Fmi2Component *comp;
//    Ctrlr *ctrl;
//};

class API_EXPORT FbsfApi
{
public:
    FbsfApi();
    ~FbsfApi() {};
    static void fct(Fmi2Component *);
    void *instanciate(int argc, char **argv);
    static void *mainApi(int argc, char **argv);
    void fmi2EnterInitialisationMode(void *ptr);
    void fmi2ExitInitialisationMode(void *ptr);
    void fmi2DoStep(void *ptr);
    void fmi2CancelStep(void *ptr);
    void fmi2Terminate(void *ptr);
    void fmi2FreeInstance(void *ptr);
    void fmi2GetStatus(void *ptr);
    void fmi2GetRealStatus(void *ptr);
    void fmi2GetIntegerStatus(void *ptr);
    void fmi2GetBooleanStatus(void *ptr);
    void fmi2GetStringStatus(void *ptr);
    void fmi2SetString(void *ptr, QString str);
};

#endif // FBSFAPI_H
