#ifndef ITEMPROPERTIES_H
#define ITEMPROPERTIES_H
#include <QString>
#include "ItemParams.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// properties structure :
//
//const Param_quality aP_qual,
//const Param_type aP_type,
//const QString & aDescription,
//const QString & aUnit,
//const QVariant & aHint,
//const QVector<QVariant> & aEnumEntries = QVector<QVariant>(),
//const QVariant & aDefault = QVariant(),
//const QVariant & aMin_strict = QVariant(),
//const QVariant & aMax_strict = QVariant(),
//const QVariant & aMin_warn = QVariant(),
//const QVariant & aMax_warn = QVariant()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Property list
// Hint : to keep a list order use [A-Za-z]% prefix
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~ Simulation properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static QMap<QString, ParamProperties> sSimulationProperties=
    {
     {"timestep", ParamProperties{Param_quality::cMandatory,
                    Param_type::cDbl,
                    "Simulation timestep",
                    "s", "step iteration period in seconds",
                    QVector<QVariant>(),0.5,0.1,INFINITY,0.1,INFINITY}},
    {"A%startDate", ParamProperties{Param_quality::cOptional, Param_type::cDate,
                                 "Date dd/MM/yyyy format",
                                 "", "",
                                 QVector<QVariant>(),
                                 ""}},
    {"B%startTime", ParamProperties{Param_quality::cOptional, Param_type::cTime,
                                 "time HH:mm:ss format",
                                 "", "",
                                 QVector<QVariant>(),
                                 ""}},
    {"C%simuMpc", ParamProperties{Param_quality::cOptional, Param_type::cBool,
                    "Time depend mode", "", "true/false",
                    QVector<QVariant>(),false}},
    {"D%pastsize", ParamProperties{Param_quality::cOptional, Param_type::cInt,
                    "Time depend past size", "", "10",
                    QVector<QVariant>(),10}},
    {"E%futursize", ParamProperties{Param_quality::cOptional, Param_type::cInt,
                    "Time depend futur size", "", "10",
                    QVector<QVariant>(),10}},
    {"F%timeshift", ParamProperties{Param_quality::cOptional, Param_type::cInt,
                    "Time depend time shift", "", "1",
                    QVector<QVariant>(),1}},
    {"perfMeter", ParamProperties{Param_quality::cOptional, Param_type::cBool,
                    "Record performance time", "", "true/false",
                    QVector<QVariant>(),false}},
    {"speedfactor", ParamProperties{Param_quality::cOptional, Param_type::cDbl,
                    "timestep*factor (Fast < 1 , Slow > 1)",
                    "", "Fast < 1 , Slow > 1",
                    QVector<QVariant>(),0.05,0.,INFINITY,0.,INFINITY}},
    {"publishparam", ParamProperties{Param_quality::cOptional, Param_type::cBool,
                    "The \"tunable\" parameters, from the CAD Logic, FbsfBaseModelFMI and FMU modules, appear in the monitor list with a color code. They are editable from the interface as are unresolved importers ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"snapshotparam", ParamProperties{Param_quality::cOptional, Param_type::cBool,
                    "The \"tunable\" parameters are saved in the snapshots ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"intialimage", ParamProperties{Param_quality::cOptional, Param_type::cFilePath,
                    "Choice of the plugin to display when the simulator is initialized (\"name\" value of the desired module ex Monitor, Plotter, etc.) ",
                    "QML files (*.qml)", "example : Plugins/Simulator.qml",
                    QVector<QVariant>(),"Plugins/Simulator.qml"}},
    {"initialPlotList", ParamProperties{Param_quality::cOptional, Param_type::cFilePath,
                    "Definition file of the curve windows to be displayed when the simulator is initialized (relative to the APP_HOME variable) ",
                    "XML files (*.xml)", "",QVector<QVariant>(),""}},
    //~~~~~~~~~~~~~~~~~~~~~~~~~ ihm options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    {"z%hidePauseBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide pause button ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"z%hideStepBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide step button ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"z%hideRunBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide run button ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"z%hideSpeedBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide speed button ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"z%hideStepCrtBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide step crt button ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"z%hideSnapControlBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide snapshot button ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"z%hideNavigationBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide navigation button ",
                    "", "true/false",QVector<QVariant>(),false}},
    {"z%hideBacktrackBtn", ParamProperties{Param_quality::cOptional, Param_type::cCheckable,
                    "Hide backtrack button ",
                    "", "true/false",QVector<QVariant>(),false}},
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~ Sequence properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static QMap<QString, ParamProperties> sSequenceProperties=
    {
     {"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                    "Name of the sequence", "", "sequence1",
                    QVector<QVariant>(),
                    "default-name"}},
     {"period", ParamProperties{Param_quality::cOptional, Param_type::cDbl,
                     "iteration count during step\n(High frequency < 1 , Low frequency > 1)",
                     "", "default = 1",
                     QVector<QVariant>(),
                     1.,0.,DBL_MAX}}
    };
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~ Plugins properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static QMap<QString, ParamProperties> sPluginProperties=
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                    "Name of the module", "", "PluginXXX",QVector<QVariant>(),
                    "default-name"}},
    {"path",ParamProperties{Param_quality::cMandatory,
                        Param_type::cPath, "Path containing the Document main.qml", "",
                        "",QVector<QVariant>() ,"no-path-set"}}
    };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~ Fmu Module properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////////// Internally defined /////////
static QMap<QString, ParamProperties> sFmuModuleProperties=
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                    "Name of the module", "", "FmuXXX", QVector<QVariant>(),
                    "default-name"}},
     {"path",ParamProperties{Param_quality::cMandatory,
                         Param_type::cFilePath, "Path of the Fmu file",
                          "Fmu files (*.fmu)", "",
                         QVector<QVariant>() ,"no-path-set"}},
     {"starttime",ParamProperties{Param_quality::cOptional,
                         Param_type::cDbl, "Start time", "",
                         "start time default is 0.0",
                         QVector<QVariant>() , 0.0,0.,INFINITY}},
     {"dumpcsv",ParamProperties{Param_quality::cOptional,
                         Param_type::cBool, "Dump results to csv file", "",
                         "Output results as a .csv formatted file",
                         QVector<QVariant>() ,false}},
     {"prefixinput",ParamProperties{Param_quality::cOptional,
                         Param_type::cBool, "Input prefix and fixed parameter with module name",
                         "","",QVector<QVariant>() ,false}},
     {"prefixoutput",ParamProperties{Param_quality::cOptional,
                         Param_type::cBool, "Output prefix and tunable parameter with module name ",
                         "","",QVector<QVariant>() ,false}}
    };

// since the category is not available in all modules code
// we define a map to get category from type (default is manual)
static QMap<QString,QString> moduleCategoryMap={
    {"fmu"          ,"fmu"},
    {"ModuleLogic"  ,"visual"},
    {"ModuleGraphic","visual"}
};
// Internally defined properties
static QMap<QString, QMap<QString, ParamProperties>> paramModuleMap={
    {"fmu"          ,sFmuModuleProperties}
};

#endif // ITEMPROPERTIES_H
