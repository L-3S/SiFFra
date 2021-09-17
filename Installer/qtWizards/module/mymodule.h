#ifndef %{JS: Cpp.headerGuard('%{HdrFileName}')}
#define %{JS: Cpp.headerGuard('%{HdrFileName}')}

#include "%{GlbHdrFileName}"
#include "FbsfBaseModel.h"

class %{ProjectName}SHARED_EXPORT %{ProjectName}
        : public FBSFBaseModel
{

public:
    %{ProjectName}();
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
    return new %{ProjectName}();
}

#endif // MYMODULE_H
