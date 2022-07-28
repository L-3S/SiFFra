#include "ModuleFMI.h"

#include <QDebug>
#include <QString>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModuleFMI::ModuleFMI()
    : FBSFBaseModelFMI()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleFMI::doInit()
{
    QString file=config()["description"];
    return parseModelDescription(file);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleFMI::doStep(int ms)
{
    FmiVariable p1=parameter("Temperature");
    //qDebug() << p1.name() << p1.Int();
    FmiVariable p2=parameter("State");
    //qDebug() << p2.name() << p2.Bool();
    FmiVariable c1=parameter("Const1");
    //qDebug() << c1.name() << c1.Int();
    return FBSF_OK;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleFMI::doTerminate()
{
    return FBSF_OK;
}

