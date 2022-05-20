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
    ~FbsfControllerComponent() {};

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
    bool isTerminated = false;
};

/** @brief flag for success or failure of last operation performed */
typedef enum{
    StepSuccess,
    StepFailure
} FbsfSuccess;

/** @brief retrieving the simulator status at call time */
typedef enum{
        FbsfUninitialized, // Initialization failed, system is not ready
        FbsfReady, // Simulator is ready to compute
        FbsfProcessing,   // Simulator is computing a step, interaction
     // is unsafe and should be prevented
        FbsfTimeOut,     // The last run step is finished and
                       // resulted in a timeout
        FbsfFailedStep,  // The last computation step is finished and failed
        FbsfTerminated,  // The ‘terminate’ function was called
} FbsfStatus;

typedef enum {
    FbsfInt,
    FbsfReal,
    FbsfVectorInt,
    FbsfVectorReal,

} FbsfDataType;

#define FbsfTrue 1
#define FbsfFalse 0

typedef void*           FbsfComponent;
typedef void*           FbsfComponentEnvironment;
typedef void*           FbsfFMUstate;
typedef void*           FbsfValueReference;
typedef int             FbsfInteger;
typedef int             FbsfBoolean;
typedef char            FbsfChar;
typedef const FbsfChar* FbsfString;
typedef char            FbsfByte;

FbsfSuccess API_EXPORT FbsfDoStep(FbsfComponent ptr, int timeOut);

/** @brief create a fmiComponent instance which is needed for all other api function */
FbsfComponent API_EXPORT FbsfInstantiate(QString str, int ac, char **av);

/** @brief Not Implemented */
FbsfSuccess API_EXPORT FbsfCancelStep(FbsfComponent ptr);

/** @brief Informs the app that the simulation run is terminated
*
This function will make the Qt application quit. */
FbsfSuccess API_EXPORT FbsfTerminate(FbsfComponent ptr);

/** @brief Free all used memory
*
Disposes the given instance, unloads the loaded model, and frees all the allocated memory and
other resources that have been allocated by the functions of the FMU interface.
If a null pointer is provided for “c”, the function call is ignored (does not have an effect). */
FbsfSuccess API_EXPORT FbsfFreeInstance(FbsfComponent *ptr);

/** @brief Get status as a FbsfStatus

Only work for FbsfStatusKind::FbsfDoStepStatus */
FbsfSuccess API_EXPORT FbsfGetStatus(FbsfComponent ptr, FbsfStatus *value);

FbsfSuccess API_EXPORT FbsfGetRealData(FbsfComponent ptr, QString name, double *val);
FbsfSuccess API_EXPORT FbsfGetIntegerData(FbsfComponent ptr, QString name, int *val);
FbsfSuccess API_EXPORT FbsfGetVectorRealData(FbsfComponent ptr, QString name, QVector<double> *val);
FbsfSuccess API_EXPORT FbsfGetVectorIntegerData(FbsfComponent ptr, QString name, QVector<int> *val);

FbsfSuccess API_EXPORT FbsfGetDataNames(FbsfComponent ptr, QStringList *list);
FbsfSuccess API_EXPORT FbsfGetDataType(FbsfComponent ptr, QString name, FbsfDataType *type);
FbsfSuccess API_EXPORT FbsfGetDataSize(FbsfComponent ptr, QString name, int *type);

FbsfSuccess API_EXPORT FbsfSaveState(FbsfComponent ptr);
FbsfSuccess API_EXPORT FbsfRestoreState(FbsfComponent ptr);
/** @brief used to get an fbsfApplication instance */
FbsfComponent API_EXPORT mainApi(int argc, char **argv);


/** @brief the Main loop of the fbsfApp in the api */
void API_EXPORT mainLoop(FbsfControllerComponent *);

#endif // FBSFAPI_H
