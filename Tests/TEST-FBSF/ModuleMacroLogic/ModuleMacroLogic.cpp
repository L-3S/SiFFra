#include "ModuleMacroLogic.h"

#include <QDebug>
#include <QString>
#include <QObject>
#include <QQuickView>
#include <QQuickItem>
#include <QDir>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModuleMacroLogic::ModuleMacroLogic()
    : FBSFBaseModel()
    , pastsize(0)
    , futursize(0)
    , timeshift(0)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMacroLogic::doInit()
{
    // get the time vector parameters
    pastsize=AppConfig()["pastsize"].toInt();
    futursize=AppConfig()["futursize"].toInt();
    timeshift=AppConfig()["timeshift"].toInt();

    qInfo() << "\t\t"
            << "pastsize/futursize/timeshift :"
            << pastsize <<futursize << timeshift;
    // load the Logic document in the visual engine
    int ret=0;
    ret = loadVisualDocument(QDir::currentPath()+"/"+config()["document"]);
    if(ret==1) initializeVisualDocument();
    return ret;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMacroLogic::doStep(int ms)
{
static int stepnum=0;
    // apply loop behavior according time parameters
    for(int i=0;i<pastsize+futursize;i++)  computeVisualDocument();
//    if(++stepnum%2==0)
//        saveStateVisualDocument();

//    restoreStateVisualDocument();
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMacroLogic::doTerminate()
{
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Executed when application request dump state
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMacroLogic::doSaveState(QDataStream& out)
{
    return saveStateVisualDocument(out);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Executed when application request reload state
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMacroLogic::doRestoreState(QDataStream& in)
{
    return restoreStateVisualDocument(in);
}
