#ifndef ModuleMPC_H
#define ModuleMPC_H
#include <QtCore/qglobal.h>

#if defined(ModuleMPC_LIBRARY)
#  define ModuleMPC_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ModuleMPC_SHARED_EXPORT Q_DECL_IMPORT
#endif

#include "FbsfBaseModel.h"
#include <QVector>

class ModuleMPC_SHARED_EXPORT ModuleMPC
        : public FBSFBaseModel
{
public:
    ModuleMPC();
    int                             doInit() override;
    int                             doTerminate() override;
    int                             doStep(int ms) override;
    QMap<QString, ParamProperties>  getParamList() override {return mListParam;}

private:
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// model variables

    int                 mTimeStep;          // seconds

    int                 unresolvedInt;
    float               unresolvedFloat;

    int                 counter1;
    float               counter2;
    QVector<int>        TimeData;

    QVector<int>        VectorIntOut;
    QVector<QVector<real>>       ContainerVectorRealOut;
    QVector<int>        VectorIntIn;
    QVector<real>       VectorRealIn;

    int                 vectornumber;
    int                 pastsize;
    int                 futursize;
    int                 timeshift;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QMap<QString, ParamProperties> mListParam=
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                              "Name of the module", "", "Test1",QVector<QVariant>(),
                              "default-name"}},
     {"vectornumber", ParamProperties{Param_quality::cOptional, Param_type::cInt,
                                      "vector number",
                                      "", "",
                                      QVector<QVariant>(),
                                      2,0,INT_MAX }},
    };
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
};

extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new ModuleMPC();
}
#endif // ModuleMPC_H
