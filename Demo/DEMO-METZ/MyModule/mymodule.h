#ifndef MYMODULE_H
#define MYMODULE_H

#include "mymodule_global.h"
#include "FbsfBaseModel.h"

class MYMODULESHARED_EXPORT MyModule
        : public FBSFBaseModel
{

public:
    MyModule();
    int     doInit();
    int     doTerminate();
    int     doStep();
    int     doTransition(FBSFBaseModel::Transition aEvent);

private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // model variables
    real realIn;
    real realOut;
    int intIn;
    int intOut;

    QVector<int> vectorInt;
    QVector<real> vectorFloat;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
};

extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new MyModule();
}

#endif // MYMODULE_H
