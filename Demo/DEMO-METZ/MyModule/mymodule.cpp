#include "mymodule.h"
#include <QDebug>
#include <QString>

MyModule::MyModule()
    : FBSFBaseModel()
    , realIn(0.1)
    , realOut(0.2)
    , intIn(0)
    , intOut(2)
    , vectorInt(10)
    , vectorFloat(10)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int MyModule::doInit()
{
    subscribe("PublicNameRin",&realIn,"%","Valve1 command");
    subscribe("PublicNameIin",&intIn,"%","Valve2 command");


    publish("PublicNameRout",&realOut,"%","Tank level");
    publish("PublicNameIout",&intOut,"%","Valve1 position");

    state("VectorInt",&vectorInt);
    state("VectorFloat",&vectorFloat);

    return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int MyModule::doStep()
{
    return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int MyModule::doTerminate()
{
    return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Transition for snapshots
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int MyModule::doTransition(FBSFBaseModel::Transition aEvent)
{
    switch(aEvent)
    {
    case cPostRestoreState :
        break;
    }
    return 1;
}