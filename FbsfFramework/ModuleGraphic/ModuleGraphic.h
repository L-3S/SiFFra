#ifndef MODULE_GRAPHIC_H
#define MODULE_GRAPHIC_H

#include "ModuleGraphic_global.h"
#include "FbsfBaseModel.h"

class MODULE_GRAPHIC_SHARED_EXPORT ModuleGraphic
        : public FBSFBaseModel
{
public:
    ModuleGraphic();
    int     doInit();
    int     doStep();
    int     doSaveState(QDataStream& out);
    int     doRestoreState(QDataStream& in);
    int     doTerminate();

    QMap<QString, ParamProperties> getParamList(){return mListParam;};



signals:

public slots:

private:
    QMap<QString, ParamProperties> mListParam =
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                              "Name of the module", "", "Graphic1",
                              QVector<QVariant>(),"default-name"}},
     {"document",ParamProperties{Param_quality::cMandatory,
                                 Param_type::cFilePath, "Path to the qml Document",
                                 "QML files (*.qml)", "",
                                 QVector<QVariant>() ,"no-path-set"}}
    };

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // model variables

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
};

extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new ModuleGraphic();
}
#endif // MODULE_LOGIC_H
