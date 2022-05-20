#ifndef ModuleTEST_H
#define ModuleTEST_H
#include <QtCore/qglobal.h>

#if defined(ModuleTEST_LIBRARY)
#  define ModuleTEST_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ModuleTEST_SHARED_EXPORT Q_DECL_IMPORT
#endif
#include "FbsfBaseModel.h"
#include <QVector>

class ModuleTEST_SHARED_EXPORT ModuleTEST
        : public FBSFBaseModel
{
public:
    ModuleTEST();
    int                             doInit() override;
    int                             doTerminate() override;
    int                             doStep(int timeOut)override;
    QMap<QString, ParamProperties>  getParamList() override {return mListParam;}

private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // model variables

    int                 unresolvedInt;
    float               unresolvedFloat;

    int                 counter1;
    float               param1;
    float               pa;
    float               paa = 0;
    float               FmuOutput;
    float               random_pos;
    float               random_neg;

    QVector<int>        VectorInt;
    QVector<int>        *VectorIntPt;
    QVector<float>      VectorReal;

    int                 vectornumber;

    QVector<int>        UIVectorIntIn;      // from QML side
    QVector<float>      UIVectorRealIn;     // from QML side
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QMap<QString, ParamProperties> mListParam=
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                              "Name of the module", "", "Plugin1",QVector<QVariant>(),
                              "default-name"}},
     {"vectornumber", ParamProperties{Param_quality::cOptional, Param_type::cInt,
                                      "vector number",
                                      "", "",
                                      QVector<QVariant>(),
                                      1,0,INT_MAX }},
     {"datetime", ParamProperties{Param_quality::cOptional, Param_type::cDateAndTime,
                                  "Date and time yyyy MM dd HH:mm:ss format",
                                  "", "",
                                  QVector<QVariant>(),
                                  "2015 03 25 12:00:00"}},
     {"datetimeUTC", ParamProperties{Param_quality::cOptional, Param_type::cDateAndTimeUTC,
                                     "Date and time UTC yyyy-MM-ddTHH:mm:ssZ format",
                                     "", "",
                                     QVector<QVariant>(),
                                     "2015-03-25T12:00:00Z"}},
     {"stringList",ParamProperties{Param_quality::cOptional,
                                   Param_type::cEnumString, "Test a selection", "",
                                   "",
                                   QVector<QVariant>({"select1","select2"})
                                   ,"select2"}},
     {"intList",ParamProperties{Param_quality::cOptional,
                                Param_type::cEnumString, "Test a selection", "",
                                "",
                                QVector<QVariant>({"1","2","3"}),
                                "2"}}
    };
};

extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new ModuleTEST();
}
#endif // ModuleTEST_H
