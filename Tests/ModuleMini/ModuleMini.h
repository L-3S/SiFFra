#ifndef ModuleMINI_H
#define ModuleMINI_H
#include <QtCore/qglobal.h>

#if defined(ModuleMini_LIBRARY)
#  define ModuleMini_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ModuleMini_SHARED_EXPORT Q_DECL_IMPORT
#endif
#include "FbsfBaseModel.h"
#include <QVector>

class ModuleMini_SHARED_EXPORT ModuleMini
        : public FBSFBaseModel
{
public:
    ModuleMini();
    int                             doInit() override;
    int                             doTerminate() override;
    int                             doStep(int timeOut)override;
    QMap<QString, ParamProperties>  getParamList() override {return mListParam;}
    int doRestoreState() override;
    int doSaveState() override;
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // model variables

    float               mTau_Inertia;
    float               mV_Delay;
    float               mSetSpeed_IN;
    float               mSpeed_OUT;
    float               mSwitch_IN;
    float               mState_OUT;
    int                 mLatency;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QMap<QString, ParamProperties> mListParam=
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                              "Name of the module", "", "Plugin1",QVector<QVariant>(),
                              "default-name"}}
    };
};

extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new ModuleMini();
}
#endif // ModuleMini_H
