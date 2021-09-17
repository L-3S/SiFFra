#include "FbsfUtilities_plugin.h"
#include "FbsfFileIO.h"
#include "FbsfMessageBox.h"
#include "FbsfSolverLogic.h"

#include <qqml.h>

void FbsfUtilitiesPlugin::registerTypes(const char *uri)
{
    // @uri FbsfUtilities
    qmlRegisterType<FbsfFileIO>(uri, 1, 0, "FbsfFileIO");
    qmlRegisterType<FbsfMessageBox>(uri, 1, 0, "FbsfMessageBox");
//    qmlRegisterType<FbsfSolverLogic>(uri, 1, 0, "FbsfSolverLogic");
}


