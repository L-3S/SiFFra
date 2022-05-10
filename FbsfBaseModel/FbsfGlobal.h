#ifndef FBSF_FRAMEWORK_GLOBAL_H
#define FBSF_FRAMEWORK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FBSF_BASEMODEL_LIBRARY)
#  define FBSF_BASEMODEL_EXPORT Q_DECL_EXPORT
#else
#  define FBSF_BASEMODEL_EXPORT Q_DECL_IMPORT
#endif

#if defined(FBSF_FRAMEWORK_LIBRARY)
#  define  FBSF_FRAMEWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FBSF_FRAMEWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

enum eApplicationMode {standalone, server, client, batch};
enum eExecutionMode {eCompute, eFullReplay, ePartialReplay, eBatch};

#define FBSF_VERSION "2.0"
#define FBSF_MAGIC 0xFB5F

typedef enum {
    FBSF_ERROR  = -1,
    FBSF_OK     = 1,
    FBSF_WARNING= 2,
    FBSF_FATAL  = 3,
    FBSF_TIMEOUT = 4
} fbsfStatus;

// As defined in the WinBase.h on Windows platform
#define INFINITE 0xFFFFFFFF

#define DEBUG qDebug() << __FUNCTION__

enum FbsfDataTypes
{
    cNoType         = '?',
    cInteger        = 'I',
    cReal           = 'R'
} ;

enum FbsfDataClass
{
    cScalar       = 'S',
    cVector       = 'V',
} ;
enum FbsfAppType
{
    cVariable     = 'V',
    cParameter    = 'P',
    cConstant     = 'C'
} ;

#endif // FBSF_FRAMEWORK_GLOBAL_H
