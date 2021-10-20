#ifndef FBSFAPI_H
#define FBSFAPI_H

#include <thread>
#include <QString>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <mutex>
#include <queue>
#include <iostream>

#if defined(BUILD_API)
#  define API_EXPORT Q_DECL_EXPORT
#else
#  define API_EXPORT Q_DECL_IMPORT
#endif

class FbsfApplication;
class API_EXPORT FbsfFmi2Component: public QObject {
    Q_OBJECT
public:
    int ac;
    char **av;
    FbsfFmi2Component(int, char **);
    FbsfApplication *app;
    std::thread qtThread;
    QString str;
    std::mutex mu;
    bool run;
    bool threadRunning;
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
typedef enum {
    fmi2OK,
    fmi2Warning,
    fmi2Discard,
    fmi2Error,
    fmi2Fatal,
    fmi2Pending
} fmi2Status;

typedef enum {
    fmi2DoStepStatus,
    fmi2PendingStatus,
    fmi2LastSuccessfulTime,
    fmi2Terminated
} fmi2StatusKind;

#define fmi2True 1
#define fmi2False 0

typedef void* fmi2Component;
typedef void* fmi2ComponentEnvironment;
typedef void* fmi2FMUstate;
typedef void* fmi2ValueReference;
typedef double fmi2Real;
typedef int fmi2Integer;
typedef int fmi2Boolean;
typedef char fmi2Char;
typedef const fmi2Char* fmi2String;
typedef char fmi2Byte;

class API_EXPORT FbsfApi
{
public:
    FbsfApi();
    ~FbsfApi() {};

    /** @brief create a fmiComponent instance which is needed for all other api function */
    fmi2Component instanciate(int argc, char **argv);

    /** @brief set the name of the configuration file for the simulation */
    fmi2Status fmi2SetString(fmi2Component ptr, QString str);

    /** @brief load the simulation configuration based on the argument passed in fmi2SetString
    * This function launch the thread that contains the Qt event thread
    * It does not launch the exec() function, it waits for function fmi2ExitInitialisationMode to be called.
    */
    fmi2Status fmi2EnterInitialisationMode(fmi2Component ptr);

    /** @brief Launch the Qt exec function that launches Qt app
    * This function must be called after fmi2EnterInitialisationMode has been called
    */
    fmi2Status fmi2ExitInitialisationMode(fmi2Component ptr);

    /** @brief Launch an asynchronous computation step
    * This function will return immediately as the computation is asyncronous
    * To check step status, use fmi2DoStepStatus
    */
    fmi2Status fmi2DoStep(fmi2Component ptr);

    /** @brief */
    fmi2Status fmi2CancelStep(fmi2Component ptr);

    /** @brief Informs the FMU that the simulation run is terminated
    * This function will make the Qt application quit.
    */
    fmi2Status fmi2Terminate(fmi2Component ptr);

    /** @brief Free all used memory
    * Disposes the given instance, unloads the loaded model, and frees all the allocated memory and
    * other resources that have been allocated by the functions of the FMU interface.
    * If a null pointer is provided for “c”, the function call is ignored (does not have an effect).
    */
    fmi2Status fmi2FreeInstance(fmi2Component ptr);

    /** @brief */
    fmi2Status fmi2GetStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Status *value);

    /** @brief */
    fmi2Status fmi2GetRealStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Real *value);

    /** @brief */
    fmi2Status fmi2GetIntegerStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Integer *value);

    /** @brief */
    fmi2Status fmi2GetBooleanStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Boolean *value);

    /** @brief */
    fmi2Status fmi2GetStringStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2String *value);
private:
    static fmi2Component mainApi(int argc, char **argv);
    static void fct(FbsfFmi2Component *);
    void copyStringToBuff(std::string s, fmi2String *buff) {
        if (mBuff) {
            free(mBuff);
        }
        mBuff = ((char*)calloc(s.length() + 1, sizeof(fmi2Char)));
        if (!mBuff) {
            qFatal("Can't alloc memory");
        }
        size_t i = 0;
        while(i < s.length()) {
            mBuff[i] = s[i];
            i++;
        }
        buff[0] = mBuff;
    }

    char *mBuff = nullptr;
};

#endif // FBSFAPI_H
