#ifndef FBSFAPI_H
#define FBSFAPI_H

#include <thread>
#include <QString>

#if defined(BUILD_API)
#  define API_EXPORT Q_DECL_EXPORT
#else
#  define API_EXPORT Q_DECL_IMPORT
#endif

class FbsfApplication;
class API_EXPORT Fmi2Component {
public:
    Fmi2Component(FbsfApplication *Aapp);
    FbsfApplication *app;
    std::thread th;
};

class API_EXPORT FbsfApi
{
public:
    FbsfApi();
    ~FbsfApi() {};
    void *instanciate(int argc, char **argv);
    void *mainApi(int argc, char **argv);
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
