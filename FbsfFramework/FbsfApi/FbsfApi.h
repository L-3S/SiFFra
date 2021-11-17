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
    FbsfFmi2Component(int, char **);

    // Store the ac and av arguments that are required for app instanciation
    int ac;
    char **av;
    // Fbsf app internal pointer
    FbsfApplication *app;
    // Thread used to run the qt event loop
    std::thread qtThread;
    // String containing the config file name for further loading
    QString configFileName;
    // Mutex used to insure synchronisation
    std::mutex mu;
    // Boolean used to trigger app run
    bool run;
    // Boolean used to know if qtThread is currently running
    bool threadRunning;
};

typedef enum {
    fmi2OK,
    fmi2Warning,
    fmi2Discard,
    fmi2Error,
    fmi2Fatal,
    fmi2Pending
} fmi2Status;

typedef enum {
    fmi2DoStepStatus, // Given this as s argument, status function delivers fmi2Pending if the computation is not finished. Otherwise the function returns the result of the asynchronously executed fmi2DoStep call.
    fmi2PendingStatus, // Given this as s argument, status function delivers a string which informs about the status of the currently running asynchronous fmi2DoStep computation.
    fmi2LastSuccessfulTime, // Given this as s argument, status function delivers the end time of the last successfully completed communication step. Can be called after fmi2DoStep(..) returned fmi2Discard.
    fmi2Terminated //Given this as s argument, status function returns fmi2True, if the slave wants to terminate the simulation.
} fmi2StatusKind;

#define fmi2True 1
#define fmi2False 0

typedef void*           fmi2Component;
typedef void*           fmi2ComponentEnvironment;
typedef void*           fmi2FMUstate;
typedef void*           fmi2ValueReference;
typedef double          fmi2Real;
typedef int             fmi2Integer;
typedef int             fmi2Boolean;
typedef char            fmi2Char;
typedef const fmi2Char* fmi2String;
typedef char            fmi2Byte;

class API_EXPORT FbsfApi
{
public:
    FbsfApi();
    ~FbsfApi() {};

    /** @brief create a fmiComponent instance which is needed for all other api function */
    fmi2Component fmi2Instanciate(int argc, char **argv);

    /** @brief set the name of the configuration file for the simulation */
    fmi2Status fmi2SetString(fmi2Component ptr, QString str);

    /** @brief load the simulation configuration based on the argument passed in fmi2SetString
    *
    This function launch the thread that contains the Qt event thread
    It does not launch the exec() function, it waits for function fmi2ExitInitialisationMode to be called. */
    fmi2Status fmi2EnterInitialisationMode(fmi2Component ptr);

    /** @brief Launch the Qt exec function that launches Qt app
    *
    This function must be called after fmi2EnterInitialisationMode has been called */
    fmi2Status fmi2ExitInitialisationMode(fmi2Component ptr);

    /** @brief Launch an asynchronous computation step
    *
    This function will return immediately as the computation is asyncronous
    To check step status, use fmi2DoStepStatus */
    fmi2Status fmi2DoStep(fmi2Component ptr);

    /** @brief Not Implemented */
    fmi2Status fmi2CancelStep(fmi2Component ptr);

    /** @brief Informs the app that the simulation run is terminated
    *
    This function will make the Qt application quit. */
    fmi2Status fmi2Terminate(fmi2Component ptr);

    /** @brief Free all used memory
    *
    Disposes the given instance, unloads the loaded model, and frees all the allocated memory and
    other resources that have been allocated by the functions of the FMU interface.
    If a null pointer is provided for “c”, the function call is ignored (does not have an effect). */
    fmi2Status fmi2FreeInstance(fmi2Component ptr);

    /** @brief Get status as a fmi2Status

    Only work for fmi2StatusKind::fmi2DoStepStatus */
    fmi2Status fmi2GetStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Status *value);

    /** @brief Get status as a fmi2Real
    *
    Only work for fmi2StatusKind::fmi2LastSuccessfulTime */
    fmi2Status fmi2GetRealStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Real *value);

    /** @brief Get status as a fmi2Integer
    *
    Work for none of the fmi2StatusKind */
    fmi2Status fmi2GetIntegerStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Integer *value);

    /** @brief Get status as a fmi2Boolean
    *
    Only work for fmi2StatusKind::fmi2Terminated */
    fmi2Status fmi2GetBooleanStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2Boolean *value);

    /** @brief Get status as a fmi2String
    *
    Only work for fmi2StatusKind::fmi2PendingStatus
    Warning: According to the fmi documentation,
    it is the users responsability to copy the content of the 'value' buffer for further use,
    the pointed memory space is an internal allocated buffer that could be reused in any further fmi call and is not safe to use */
    fmi2Status fmi2GetStringStatus(fmi2Component ptr, const fmi2StatusKind s, fmi2String *value);
private:
    /** @brief used to get an fbsfApplication instance */
    static fmi2Component mainApi(int argc, char **argv);

    /** @brief the Main loop of the fbsfApp in the api */
    static void mainLoop(FbsfFmi2Component *);

    /** @brief Fill the internal buffer mBuff and makes the arg buff points to it
    *
    Warning: According to the fmi documentation,
    it is the users responsability to copy the content of the value buffer for further use,
    the pointed memory space is an internal allocated buffer that could be reused in any further fmi call and is not safe to use */
    void copyStringToBuff(std::string s, fmi2String *buff) {
        if (mBuff) {
            free(mBuff);
        }
        mBuff = ((char*)calloc(s.length() + 1, sizeof(fmi2Char)));
        if (!mBuff) {
            qFatal("Can't alloc memory");
            return;
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
