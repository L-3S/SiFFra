#ifndef MODULE_LOGIC_H
#define MODULE_LOGIC_H

#include "ModuleMacroLogic_global.h"
#include "FbsfBaseModel.h"

class MODULE_LOGIC_SHARED_EXPORT ModuleMacroLogic
        : public FBSFBaseModel
{
public:
    ModuleMacroLogic();
    int     doInit();
    int     doTerminate();
    int     doStep(int ms);
    int     doSaveState(QDataStream &out);      // Save states to file
    int     doRestoreState(QDataStream &in);    // Restore states from file

signals:

public slots:

private:

    int                 pastsize;
    int                 futursize;
    int                 timeshift;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// model variables

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
};

extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new ModuleMacroLogic();
}
#endif // MODULE_LOGIC_H
