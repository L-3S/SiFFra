#ifndef FmuWrapper_H
#define FmuWrapper_H


#include "FbsfBaseModel.h"
#include "FbsfGlobal.h"
#include "fmi2.h"

#include <QList>
#include <QFile>
#include <QByteArray>
#include <QLoggingCategory>

// Used 7z options, version 4.57:
// -x   Extracts files from an archive with their full paths in the current dir, or in an output dir if specified
// -aoa Overwrite All existing files without prompt
// -o   Specifies a destination directory where files are to be extracted
#define UNZIP_CMD "7z x -aoa -o"
#define XML_FILE  "modelDescription.xml"
#define RESULT_FILE "result.csv"

#ifdef _WIN64
#define DLL_DIR   "/binaries/win64/"
#else
#define DLL_DIR   "/binaries/win32/"
#endif

// return codes of the 7z command line tool
#define SEVEN_ZIP_NO_ERROR 0 // success
#define SEVEN_ZIP_WARNING 1  // e.g., one or more files were locked during zip
#define SEVEN_ZIP_ERROR 2
#define SEVEN_ZIP_COMMAND_LINE_ERROR 7
#define SEVEN_ZIP_OUT_OF_MEMORY 8
#define SEVEN_ZIP_STOPPED_BY_USER 255

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Class for FMU public variables
//  variables are used for copy from and to Public Data
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_BASEMODEL_EXPORT FmuVariable
{
public :
    QString             name;
    fmi2ValueReference  reference;
    Elm                 type;
    QString             causality;
    QString             variability;
    QString             unit;
    QString             description;
    bool                isState;
    union
    {
        int     mInteger;
        float   mReal;
    } value;

};
class ParamProperties;
///~~~~~~~~~~~~~~~~~~~ FmuWrapper declaration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_BASEMODEL_EXPORT FmuWrapper
     : public FBSFBaseModel
{

public:
    FmuWrapper(float aStep, double aStart, double aEnd,
               bool aCsvOutput=false, fmi2Boolean aLoggingOn=false,
               int aNCategories=0, char **aCategories=NULL);

    int                    loadFMU(QString fmuFileName);
    int                    initialConditions();

    int                    doInit();
    int                    doTerminate();
    int                    doStep();
    int                    doTransition(FBSFBaseModel::Transition aEvent);
    int                    doSaveState();
    int                    doRestoreState();
    int                    doSaveState(QDataStream &out);     // dump state to file
    int                    doRestoreState(QDataStream& in);   // Restore state from file
    QMap<QString, ParamProperties> getParamList(){return {};}; //TODO

protected :
    void                   defineFmiVariables();
    virtual void           registerFmiVariable();
    /// pushFromModelDescriptor:
    /// force value from ModelDescriptor to internal state
    void                   pushFromModelDescriptor();
    void                   timeStep(double aTimeStep) {mStep=aTimeStep;} // FMU Time step modifier
    fmi2Component&         getComponent() { return component;} // reference to component

private :
    void                   printModelDescription(ModelDescription* md);
    int                    loadDll(QString& dllPath);

    int                    outputToCsv();

    static void            fmuLogger(  void *componentEnvironment,
                                       fmi2String instanceName, fmi2Status status,
                                       fmi2String category, fmi2String message, ...);
    static void            replaceRefsInMessage( FMU* fmu,const char* msg, char* buffer, int nBuffer);
    static ScalarVariable* getSV(FMU* fmu, char type, fmi2ValueReference vr);
    static const char*     fmi2StatusToString(fmi2Status status);

///~~~~~~~~~~~~~~~ FBSFBaseModel interface ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
signals:

public slots:

protected :
    FMU fmu;                            // the fmu to simulate
    QList<FmuVariable*> yOutputs;       // List of fmi output variables
    QList<FmuVariable*> yInputs;        // List of fmi input variables
    QList<FmuVariable*> yStates;        // List of fmi state variables
    QList<FmuVariable*> yParameters;    // List of fmi parameters

private :
    float time;                         // Current simulated time
    float tStart;                       // Start time
    float tEnd;                         // End time
    double mStep;                       // model time step

    const char* guid;                   // global unique id of the fmu
    const char *instanceName;           // instance name
    fmi2Component component;            // instance of the fmu
    fmi2Status   fmiFlag;               // return code of the fmu functions
    QString fmuDir;
    const char* fmuResourceLocation;    // path to the fmu resources as URL, "file://C:\Vendor\sales"
    fmi2Boolean  visible;               // no simulator user interface

    fmi2CallbackFunctions callbacks;    // called by the model during simulation
    ModelDescription* md;               // handle to the parsed XML file
    fmi2Boolean toleranceDefined;       // true if model description define tolerance
    fmi2Real tolerance;                 // used in setting up the experiment

    int     nSteps;                     // step count
    Element *defaultExp;                // default experiment
    QFile   file;                       // csv file
    bool    csvStatus;                  // csv option flag
    bool    csvHeader;                  // one shot flag

    fmi2Boolean loggingOn;              // Logging option
    int     nCategories;                // number of categories
    char**  categories;                 // login categories

    fmi2FMUstate    mFMUstate;          // fmu state
    QByteArray      mStateStream;       // Serialization/Unserialization
    //parameter options
    bool            bPublishParam;
    bool            bSnapshotParam;


    Q_LOGGING_CATEGORY(log, "MacroFMU", QtInfoMsg)
};

#endif // FmuWrapper_H
