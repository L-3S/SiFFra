#ifndef ModuleFMI_H
#define ModuleFMI_H

#include "ModuleFMI_global.h"
#include "FbsfBaseModelFMI.h"
#include <QVector>

class ModuleFMI_SHARED_EXPORT ModuleFMI
        : public FBSFBaseModelFMI
{
public:
    ModuleFMI();
    int                     doInit() override;
    int                     doTerminate() override;
    int                     doStep(int timeoutMs) override;

    QMap<QString, ParamProperties> getParamList() override {return mListParam;}

private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // model variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    QMap<QString, ParamProperties> mListParam=
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                              "Name of the module", "", "Plugin1",QVector<QVariant>(),
                              "default-name"}},
     {"description",ParamProperties{Param_quality::cMandatory,
                                    Param_type::cFilePath,
                                    "File path of the model description", "XML files (*.xml)",
                                    "C:\\data\\Fmu\\ModelDescription.xml",
                                    QVector<QVariant>() ,"no-path-set"}}
    };
};

extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new ModuleFMI();
}
#endif // ModuleFMI_H
