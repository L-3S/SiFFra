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
class API_EXPORT FbsfControllerComponent: public QObject {
    Q_OBJECT
public:
    FbsfControllerComponent(int, char **);

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
    FbsfOK,
    FbsfWarning,
    FbsfDiscard,
    FbsfError,
    FbsfFatal,
    FbsfPending
} FbsfStatus;

typedef enum {
    FbsfDoStepStatus, // Given this as s argument, status function delivers FbsfPending if the computation is not finished. Otherwise the function returns the result of the asynchronously executed FbsfDoStep call.
    FbsfPendingStatus, // Given this as s argument, status function delivers a string which informs about the status of the currently running asynchronous FbsfDoStep computation.
    FbsfLastSuccessfulTime, // Given this as s argument, status function delivers the end time of the last successfully completed communication step. Can be called after FbsfDoStep(..) returned FbsfDiscard.
    FbsfTerminated //Given this as s argument, status function returns FbsfTrue, if the slave wants to terminate the simulation.
} FbsfStatusKind;

#define FbsfTrue 1
#define FbsfFalse 0

typedef void*           FbsfComponent;
typedef void*           FbsfComponentEnvironment;
typedef void*           FbsfFMUstate;
typedef void*           FbsfValueReference;
typedef double          FbsfReal;
typedef int             FbsfInteger;
typedef int             FbsfBoolean;
typedef char            FbsfChar;
typedef const FbsfChar* FbsfString;
typedef char            FbsfByte;

class API_EXPORT FbsfApi
{
public:
    FbsfApi();
    ~FbsfApi() {};

    /** @brief create a fmiComponent instance which is needed for all other api function */
    FbsfComponent FbsfInstanciate(int argc, char **argv);

    /** @brief set the name of the configuration file for the simulation */
    FbsfStatus FbsfSetString(FbsfComponent ptr, QString str);

    /** @brief load the simulation configuration based on the argument passed in FbsfSetString
    *
    This function launch the thread that contains the Qt event thread
    It does not launch the exec() function, it waits for function FbsfExitInitialisationMode to be called. */
    FbsfStatus FbsfEnterInitialisationMode(FbsfComponent ptr);

    /** @brief Launch the Qt exec function that launches Qt app
    *
    This function must be called after FbsfEnterInitialisationMode has been called */
    FbsfStatus FbsfExitInitialisationMode(FbsfComponent ptr);

    /** @brief Launch an asynchronous computation step
    *
    This function will return immediately as the computation is asyncronous
    To check step status, use FbsfDoStepStatus */
    FbsfStatus FbsfDoStep(FbsfComponent ptr);

    /** @brief Not Implemented */
    FbsfStatus FbsfCancelStep(FbsfComponent ptr);

    /** @brief Informs the app that the simulation run is terminated
    *
    This function will make the Qt application quit. */
    FbsfStatus FbsfTerminate(FbsfComponent ptr);

    /** @brief Free all used memory
    *
    Disposes the given instance, unloads the loaded model, and frees all the allocated memory and
    other resources that have been allocated by the functions of the FMU interface.
    If a null pointer is provided for “c”, the function call is ignored (does not have an effect). */
    FbsfStatus FbsfFreeInstance(FbsfComponent ptr);

    /** @brief Get status as a FbsfStatus

    Only work for FbsfStatusKind::FbsfDoStepStatus */
    FbsfStatus FbsfGetStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfStatus *value);

    /** @brief Get status as a FbsfReal
    *
    Only work for FbsfStatusKind::FbsfLastSuccessfulTime */
    FbsfStatus FbsfGetRealStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfReal *value);

    /** @brief Get status as a FbsfInteger
    *
    Work for none of the FbsfStatusKind */
    FbsfStatus FbsfGetIntegerStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfInteger *value);

    /** @brief Get status as a FbsfBoolean
    *
    Only work for FbsfStatusKind::FbsfTerminated */
    FbsfStatus FbsfGetBooleanStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfBoolean *value);

    /** @brief Get status as a FbsfString
    *
    Only work for FbsfStatusKind::FbsfPendingStatus
    Warning: According to the fmi documentation,
    it is the users responsability to copy the content of the 'value' buffer for further use,
    the pointed memory space is an internal allocated buffer that could be reused in any further fmi call and is not safe to use */
    FbsfStatus FbsfGetStringStatus(FbsfComponent ptr, const FbsfStatusKind s, FbsfString *value);
private:
    /** @brief used to get an fbsfApplication instance */
    static FbsfComponent mainApi(int argc, char **argv);

    /** @brief the Main loop of the fbsfApp in the api */
    static void mainLoop(FbsfControllerComponent *);

    /** @brief Fill the internal buffer mBuff and makes the arg buff points to it
    *
    Warning: According to the fmi documentation,
    it is the users responsability to copy the content of the value buffer for further use,
    the pointed memory space is an internal allocated buffer that could be reused in any further fmi call and is not safe to use */
    void copyStringToBuff(std::string s, FbsfString *buff) {
        if (mBuff) {
            free(mBuff);
        }
        mBuff = ((char*)calloc(s.length() + 1, sizeof(FbsfChar)));
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
