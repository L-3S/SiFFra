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
    Fmi2Component(int argc, char **argv);
    QThread thr;
    FbsfApplication *app;
    std::thread th;
    void launch() {
        emit operate();
    }
    void pb() {
        qDebug() << "taratata";
    }
    QString str;
    int argc;
    char **argv;
public slots:
    void run (QString &);
signals:
    void operate(uint aPeriod=100,float aFactor=1,uint recorder=0);
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
    void *instanciate(int argc, char **argv);
    FbsfApplication *mainApi(int argc, char **argv);
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
