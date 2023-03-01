#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QDataStream>
#include <QLibrary>

#include "FmuWrapper.h"
#include <iostream>

//#define TRACE
#define USE_FMUSTATE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FmuWrapper
    \brief FMU model wrapper
    Unzip the fmu file
    Publish and subscribe public data
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FmuWrapper::FmuWrapper(fmi2Real aStep,fmi2Real aStart, fmi2Real aEnd,
                       bool aCsvOutput,fmi2Boolean aLoggingOn,
                       int aNCategories, char **aCategories)
    : FBSFBaseModel()
    , time(0.0)
    , tStart(aStart)
    , tEnd(aEnd)
    , guid(nullptr)
    , instanceName(nullptr)
    , fmuResourceLocation(nullptr)
    , visible(fmi2False)
    , md(nullptr)
    , toleranceDefined(fmi2False)
    , tolerance(0)
    , nSteps(0)
    , defaultExp(nullptr)
    , mStep(aStep)
    , loggingOn(aLoggingOn)
    , nCategories(aNCategories)
    , categories (aCategories)
    , file(RESULT_FILE)
    , csvStatus(aCsvOutput)
    , csvHeader(true)
    , mFMUstate(nullptr)
{

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Load/unzip fmu component files
/// Load dll and resolve symbols
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::loadFMU(QString  fmuFileName)
{
    QString fmuPath;
    QString xmlFilePath;
    QString dllPath;
    QDir    dir;
    const char* modelId;
    const char* fmiVersion;

    // get absolute path to FMU, NULL if not found
    fmuPath = dir.absoluteFilePath(fmuFileName);
    if (!dir.exists(fmuPath))
    {
        qDebug() << fmuPath << " doesn't exist";
        return 0;// failure
    }
    QFileInfo fmuFileInfo(fmuFileName);

    // Home Dir of the .fmu file
    fmuDir = fmuFileInfo.absolutePath();

    // Xml file for the Fmu Description
    xmlFilePath = fmuDir +"/"+ XML_FILE;

    QFileInfo xmlFileInfo(xmlFilePath);
    // unzip fmu if modeldescription doesn't exist or fmu file is newer
    //if (!xmlFileInfo.exists() || fmuFileInfo.lastModified() > xmlFileInfo.lastModified())
    {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // unzip the FMU to the fmuDir directory
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        qInfo()<< "\t\tUnzip fmu bundle" << "\n\t\t\tfmu date :"
               << fmuFileInfo.lastModified().toString()
               << "\n\t\t\tModelDescription date :"
               << xmlFileInfo.lastModified().toString();

        // run the unzip command
        // remove "> NUL" to see the unzip protocol
        QString zipCmd(UNZIP_CMD);
        zipCmd +=  fmuDir + " " +fmuPath + " " + "> NUL" ;

        int code = system(zipCmd.toStdString().c_str());
        if ( code != SEVEN_ZIP_NO_ERROR)
        {
            qDebug() <<"7z: ";
            switch (code) {
            case SEVEN_ZIP_WARNING:            qDebug() << "warning\n"; return -1;
            case SEVEN_ZIP_ERROR:              qDebug() << "error\n"; return -1;
            case SEVEN_ZIP_COMMAND_LINE_ERROR: qDebug() << "command line error\n"; return -1;
            case SEVEN_ZIP_OUT_OF_MEMORY:      qDebug() << "out of memory\n"; return -1;
            case SEVEN_ZIP_STOPPED_BY_USER:    qDebug() << "stopped by user\n"; return -1;
            default:                           qDebug() << "unknown error\n";
                return 0;// failure
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // parse fmuDir\modelDescription.xml
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    fmu.modelDescription = parse(const_cast<char *>(xmlFilePath.toStdString().c_str()));

    if (!fmu.modelDescription)
    {
        qDebug() << "[Error] : error while parsing " << XML_FILE;
        return 0;// failure
    }
    else
    {
        printModelDescription(fmu.modelDescription);
    }

#ifdef FMI_COSIMULATION
    modelId = getAttributeValue((Element *)getCoSimulation(fmu.modelDescription), att_modelIdentifier);
#else // FMI_MODEL_EXCHANGE
    modelId = getAttributeValue((Element *)getModelExchange(fmu.modelDescription), att_modelIdentifier);
#endif

    // Check the FMI version
    fmiVersion = getAttributeValue((Element *)fmu.modelDescription, att_fmiVersion);
    if(strcmp(fmiVersion,"2.0")!=0)
    {
        QString msg=QString("%1 : FMI Version %2 mismatch, currently 2.0").arg(modelId).arg(fmiVersion);
        qFatal(msg.toStdString().c_str()) ;
    }

    // load the FMU dll extention .so or .dll is add by QLibrary
    dllPath += fmuDir + DLL_DIR + modelId;
    if (!loadDll(dllPath))
    {
        qDebug() << "error while loading " << dllPath;
        return 0;// failure
    }
    file.setFileName(QString(modelId)+".csv");
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Initialize the model
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doInit()
{
    //parameter options
    bPublishParam =AppConfig()["publishparam"]=="true"?true:false;
    bSnapshotParam=AppConfig()["snapshotparam"]=="true"?true:false;
    // instantiate the fmu
    md                              = fmu.modelDescription;
    guid                            = getAttributeValue((Element *)md, att_guid);
    instanceName                    = getAttributeValue((Element *)getCoSimulation(md), att_modelIdentifier);

    callbacks.logger                = &FmuWrapper::fmuLogger;
    callbacks.allocateMemory        = calloc;
    callbacks.freeMemory            = free;
    callbacks.stepFinished          = NULL; // fmiDoStep has to be carried out synchronously
    callbacks.componentEnvironment  = &fmu; // pointer to current fmu from the environment.


    // Covert QString to const char *
    QString tmpQstring = "file:///" + fmuDir +"/"+ "resources";
    std::string tmpString = tmpQstring.toStdString();
    fmuResourceLocation = tmpString.c_str();

    component                       = fmu.instantiate(  instanceName, fmi2CoSimulation,
                                                        guid, fmuResourceLocation,
                                                        &callbacks, visible, loggingOn);
    if (!component)
    {
        qDebug () << "could not instantiate model";
        return 0; // failure
    }

    if (nCategories > 0)
    {
        fmiFlag = fmu.setDebugLogging(component, fmi2True, nCategories, categories);
        if (fmiFlag > fmi2Warning)
        {
            qCritical ("could not initialize model; failed FMI set debug logging");
            return 0; // failure
        }
    }

    defaultExp = getDefaultExperiment(md);
    ValueStatus vs;
    if (defaultExp) tolerance = getAttributeDouble(defaultExp, att_tolerance, &vs);
    if (vs == valueDefined)
    {
        toleranceDefined = fmi2True;
    }

    initialConditions();

    if (csvStatus) outputToCsv();// output column names

    // register Variable reference
    registerFmiVariable();

    return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Initialize experiment for the model
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::initialConditions()
{
    fmiFlag = fmu.setupExperiment(component, toleranceDefined, tolerance, tStart, fmi2False, tEnd);
    if (fmiFlag > fmi2Warning)
    {
        qCritical ("could not initialize model; failed FMI setup experiment");
        return 0; // failure
    }
    fmiFlag = fmu.enterInitializationMode(component);
    if (fmiFlag > fmi2Warning)
    {
        qCritical ("could not initialize model; failed FMI enter initialization mode");
        return 0; // failure
    }
    fmiFlag = fmu.exitInitializationMode(component);
    if (fmiFlag > fmi2Warning)
    {
        qCritical ("could not initialize model; failed FMI exit initialization mode");
        return 0; // failure
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Manage public data and computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doStep()
{
    if (time <= tEnd)
    {
        QList<FmuVariable*>::iterator iter;
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Get input interface data pool to FMU input interface
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (iter = yInputs.begin(); iter != yInputs.end(); ++iter)
        {
            switch ((*iter)->type)
            {
            case elm_Real:
                fmi2Real vRealValue;
                vRealValue=(*iter)->value.mReal;    // cast float to double
                fmu.setReal(component, &((*iter)->reference), 1, &vRealValue);
                break;
            case elm_Boolean:
                fmi2Boolean vBoolValue;
                vBoolValue=(*iter)->value.mInteger;
                fmu.setBoolean(component, &((*iter)->reference), 1, &vBoolValue);
                break;
            case elm_Integer:
                fmu.setInteger(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                break;
            default : break;

            }
        }
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Compute model step from time to time+mStep
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        fmiFlag = fmu.doStep(component, time, mStep, fmi2True);
        if (fmiFlag > fmi2Warning)
        {
            qCritical ("could not complete simulation of the model");
            return 0;// failure
        }
        time += mStep;
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // push FMU output Interface to output interface data pool
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (iter = yOutputs.begin(); iter != yOutputs.end(); ++iter)
        {
            switch ((*iter)->type)
            {
            case elm_Real:
                fmi2Real vRealValue;
                fmu.getReal(component, &((*iter)->reference), 1, &vRealValue);

                (*iter)->value.mReal=(float)vRealValue;// cast double to float
                break;
            case elm_Boolean:
                fmu.getBoolean(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                break;
            case elm_Integer:
                fmu.getInteger(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                break;
            default : break;
            }
        }
        nSteps++;
        if (csvStatus) outputToCsv();// output solution for time t
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Execute simulation state transition
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doTransition(FBSFBaseModel::Transition aEvent)
{
    switch(aEvent)
    {
    case cPostRestoreState : // trigger post loading snapshot
    {
        //DEBUG << name() << "cPostRestoreState ";

        QList<FmuVariable*>::iterator iter;
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Push input from data pool to FMU input interface
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (iter = yOutputs.begin(); iter != yOutputs.end(); ++iter)
        {
            switch ((*iter)->type)
            {
            case elm_Real:
                fmi2Real vRealValue;

                vRealValue=(*iter)->value.mReal;// cast float to double
                fmu.setReal(component, &((*iter)->reference), 1, &vRealValue);
                //DEBUG << "Output"<< (*iter)->name << vRealValue;
                break;
            case elm_Boolean:
                fmu.setBoolean(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                break;
            case elm_Integer:
                //DEBUG << "Output"<< (*iter)->name <<(*iter)->value.mInteger;

                fmu.setInteger(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                break;
            default : break;
            }
        }
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Push states interface to FMU states interface
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (iter = yStates.begin(); iter != yStates.end(); ++iter)
        {
            switch ((*iter)->type)
            {
            case elm_Real:
                fmi2Real vRealValue;
                vRealValue=(*iter)->value.mReal;// cast float to double
                fmu.setReal(component, &((*iter)->reference), 1, &vRealValue);
                //DEBUG << "State"<< (*iter)->name <<  (*iter)->causality << vRealValue;
                break;
            case elm_Boolean:
                fmu.setBoolean(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
            case elm_Integer:
                fmu.setInteger(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                //DEBUG << "State"<< (*iter)->name <<  (*iter)->causality << (*iter)->value.mInteger;
                break;
            default : break;
            }
        }
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Push parameter from data pool to FMU parameter interface
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(bSnapshotParam==true)
        {
            for (iter = yParameters.begin(); iter != yParameters.end(); ++iter)
            {
                switch ((*iter)->type)
                {
                case elm_Real:
                    fmi2Real vRealValue;

                    vRealValue=(*iter)->value.mReal;// cast float to double
                    fmu.setReal(component, &((*iter)->reference), 1, &vRealValue);
                    publishValue((*iter)->name,(*iter)->value.mReal);
                    //DEBUG << "Parameter"<< (*iter)->name << vRealValue;
                    break;
                case elm_Boolean:
                    fmu.setBoolean(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                    publishValue((*iter)->name,(*iter)->value.mInteger);
                case elm_Integer:
                    //DEBUG << "Parameter"<< (*iter)->name <<(*iter)->value.mInteger;
                    fmu.setInteger(component,&((*iter)->reference), 1, &((*iter)->value.mInteger));
                    publishValue((*iter)->name,(*iter)->value.mInteger);
                    break;
                default : break;
                }
            }
        }
    }
        break;
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Finalize computation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doTerminate()
{
    // end simulation
    fmu.terminate(component);
    fmu.freeInstance(component);

    freeModelDescription(fmu.modelDescription);
    if (categories) free(categories);
    if (csvStatus)
    {
        file.close();
    }
    // print simulation summary
    qInfo() << "FMU" << name();
    qInfo() << "Simulation from " << tStart << "to" << tEnd << "terminated successful";
    qInfo() << "  steps ............ " << nSteps;
    qInfo() << "  fixed step size .. " << mStep;
    return 1; // success
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// save FMU states, inputs [outputs and states] (API level)
/// Override FBSFBaseModel::doSaveState()
/// save sequence :
///     getFMUstate
///     save time
///     save inputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doSaveState()
{
    bool bStateMode = getAttributeValue((Element *)getCoSimulation(fmu.modelDescription), att_canGetAndSetFMUstate);
    if (!bStateMode)
    {
        QString msg=name() + " : Cannot get and set FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
    fmiFlag = fmu.getFMUstate (component,&mFMUstate);
    if (fmiFlag > fmi2Warning)
    {
        QString msg=name() + " : Failed to get FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
#ifndef USE_FMUSTATE
    FBSFBaseModel::doSaveState(); // save declared output and states
#else
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // save FMU time to memory map
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    mStateDataValueMap[name()+"::time" ]=time;
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // save FMU inputs to memory map
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QList<FmuVariable*>::iterator iter;

    for (iter = yInputs.begin(); iter != yInputs.end(); ++iter)
    {
        switch ((*iter)->type)
        {
        case elm_Real:
            mStateDataValueMap[(*iter)->name ]=(*iter)->value.mReal;
            break;
        case elm_Boolean:
            mStateDataValueMap[(*iter)->name ]=(*iter)->value.mInteger;
            break;
        case elm_Integer:
            mStateDataValueMap[(*iter)->name ]=(*iter)->value.mInteger;
            break;
        default : break;
        }
    }
#endif
    return fmiFlag != fmi2OK ? 0 : 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// restore FMU states, inputs [outputs and states]  (API level)
/// Override FBSFBaseModel::doRestoreState()
/// restore sequence
///     setFMUstate
///     restore time
///     restore inputs
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doRestoreState()
{
    bool bStateMode = getAttributeValue((Element *)getCoSimulation(fmu.modelDescription), att_canGetAndSetFMUstate);
    if (!bStateMode)
    {
        QString msg=name() + " : Cannot get and set FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
#ifndef USE_FMUSTATE
    FBSFBaseModel::doRestoreState(); // restore declared outputs and states
#endif

    if (fmiFlag > fmi2Error)
    {
        QString msg=name() + " : Failed to get FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
    fmiFlag = fmu.setFMUstate (component,mFMUstate);
    if (fmiFlag > fmi2Warning)
    {
        QString msg=name() + " : Failed to set FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
#ifdef USE_FMUSTATE
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // restore FMU time from memory map
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    time=mStateDataValueMap[name()+"::time" ].toFloat();
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // restore inputs to FMU interface from memory map
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QList<FmuVariable*>::iterator iter;
    for (iter = yInputs.begin(); iter != yInputs.end(); ++iter)
    {
        // push back the value to the intermediate variable
        switch ((*iter)->type)
        {
        case elm_Real:
            (*iter)->value.mReal=mStateDataValueMap[(*iter)->name].toFloat();
            break;
        case elm_Boolean:
            (*iter)->value.mInteger=mStateDataValueMap[(*iter)->name].toInt();
            break;
        case elm_Integer:
            (*iter)->value.mInteger=mStateDataValueMap[(*iter)->name].toInt();
            break;
        default : break;
        }
    }
#endif
    return fmiFlag != fmi2OK ? 0 : 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Serialize model states inputs, outputs and states (File level)
/// - ask for serialization size and create dummy vector
/// - ask for FMU state and then serialize content of FMU
/// Sequence :
/// FBSFBaseModel::doSaveState (QDataStream &out)
///     FmuWrapper::doSaveState()
///         Serialize()
///         FBSFBaseModel::doSaveState()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doSaveState(QDataStream &out)
{
    bool bStateMode = getAttributeValue((Element *)getCoSimulation(fmu.modelDescription),
                                        att_canSerializeFMUstate);
    if (!bStateMode)
    {
        QString msg=name() + " : Cannot Serialize FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
    // get Fmu state and save inputs to map
    doSaveState();
    // get the Fmu state data size
    size_t  vStateSize=0;
    fmiFlag = fmu.serializedFMUstateSize(component,mFMUstate,&vStateSize);
    if (fmiFlag > fmi2Warning)
    {
        QString msg=name() + " : Failed to get serialized FMUstate size";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
    // get and store the Fmu state data buffer
    mStateStream.resize(vStateSize);
    fmiFlag = fmu.serializeFMUstate(component,mFMUstate,mStateStream.data(),vStateSize);
    if (fmiFlag > fmi2Warning)
    {
        QString msg=name() + " : Failed to serialize FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
#ifdef TRACE
    DEBUG << name() << fmi2StatusToString(fmiFlag)
          << "State Size:" << mStateStream.size();
#endif
    // push stored Fmu data to map
    mStateDataValueMap["FmuState"]=mStateStream;
    // call the base class
    FBSFBaseModel::doSaveState(out);

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Deserialize model states inputs, outputs and states (File level)
/// - do normal FMU init
/// - get serialized data
/// - tell FMU to de-serialize state
/// Sequence :
/// FBSFBaseModel::doRestoreState (QDataStream &in)
///     FmuWrapper::doRestoreState()
///         Deserialize()
///         FBSFBaseModel::doRestoreState()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::doRestoreState(QDataStream& in)
{
#if false
    // TEST ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    size_t          StateSize=0;
    static fmi2FMUstate    FMUstate;
    fmiFlag = fmu.getFMUstate (component,&FMUstate);
    fmiFlag = fmu.serializedFMUstateSize(component,FMUstate,&StateSize);
    char* buff=new char[StateSize];
    fmiFlag = fmu.serializeFMUstate(component,FMUstate,buff,StateSize);
    fmiFlag = fmu.deSerializeFMUstate(component,buff, StateSize, &FMUstate);
    fmu.setFMUstate (component,FMUstate);
    return 1;
#endif


    bool bStateMode = getAttributeValue((Element *)getCoSimulation(fmu.modelDescription), att_canSerializeFMUstate);
    if (!bStateMode)
    {
        QString msg=name() + " : Cannot Deserialize FMUstate";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
    // call the base class
    FBSFBaseModel::doRestoreState(in);

    // get the stored state data from map
    // if the variant has userType() QMetaType::QByteArray
    QByteArray vStateStream=mStateDataValueMap["FmuState"].toByteArray();

    //~~~~~~~~ get the FMU state data size to check if the size match ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    size_t  vStateSize=0;
    fmiFlag = fmu.getFMUstate (component,&mFMUstate);
    fmiFlag = fmu.serializedFMUstateSize(component,mFMUstate,&vStateSize);
    if (vStateSize != vStateStream.size())
    {
        QString msg;
        msg=QString("%1 : mismatch FMUstate size file (%2) fmu (%3)")
                .arg(name()).arg(vStateStream.size()).arg(vStateSize);
        qCritical (msg.toStdString().c_str());
        return 0;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    fmiFlag = fmu.deSerializeFMUstate(component,vStateStream.constData(), vStateStream.size(), &mFMUstate);
    if (fmiFlag > fmi2Warning)
    {
        QString msg=name() + " : Failed to deserialize FmuState";
        qCritical (msg.toStdString().c_str());
        return 0;
    }
#ifdef TRACE
    DEBUG << name() << fmi2StatusToString(fmiFlag);
    DEBUG << "Time" << time;
    DEBUG << "State Size:" << vStateStream.size();
#endif
    // set Fmu state and Fmu inputs
    // doRestoreState(); // fail with setFmuState

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Load the given dll and set function pointers in fmu
/// Return 0 to indicate failure
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::loadDll(QString& dllPath)
{
    int s = 1;


    // Load the Fmu dll or .so without the need of the unload
    // cause this happens automatically on application termination
    QLibrary sharedLibrary(dllPath);

    fmu.getTypesPlatform = reinterpret_cast<fmi2GetTypesPlatformTYPE *>(sharedLibrary.resolve("fmi2GetTypesPlatform"));
    fmu.getVersion = reinterpret_cast<fmi2GetVersionTYPE *>(sharedLibrary.resolve("fmi2GetVersion"));
    fmu.setDebugLogging = reinterpret_cast<fmi2SetDebugLoggingTYPE *>(sharedLibrary.resolve("fmi2SetDebugLogging"));
    fmu.instantiate = reinterpret_cast<fmi2InstantiateTYPE *>(sharedLibrary.resolve("fmi2Instantiate"));
    fmu.freeInstance = reinterpret_cast<fmi2FreeInstanceTYPE *>(sharedLibrary.resolve("fmi2FreeInstance"));
    fmu.setupExperiment = reinterpret_cast<fmi2SetupExperimentTYPE *>(sharedLibrary.resolve("fmi2SetupExperiment"));
    fmu.enterInitializationMode = reinterpret_cast<fmi2EnterInitializationModeTYPE *>(sharedLibrary.resolve("fmi2EnterInitializationMode"));
    fmu.exitInitializationMode = reinterpret_cast<fmi2ExitInitializationModeTYPE *>(sharedLibrary.resolve("fmi2ExitInitializationMode"));
    fmu.terminate = reinterpret_cast<fmi2TerminateTYPE *>(sharedLibrary.resolve("fmi2Terminate"));
    fmu.reset = reinterpret_cast<fmi2ResetTYPE *>(sharedLibrary.resolve("fmi2Reset"));
    fmu.getReal = reinterpret_cast<fmi2GetRealTYPE *>(sharedLibrary.resolve("fmi2GetReal"));
    fmu.getInteger = reinterpret_cast<fmi2GetIntegerTYPE *>(sharedLibrary.resolve("fmi2GetInteger"));
    fmu.getBoolean = reinterpret_cast<fmi2GetBooleanTYPE *>(sharedLibrary.resolve("fmi2GetBoolean"));
    fmu.getString = reinterpret_cast<fmi2GetStringTYPE *>(sharedLibrary.resolve("fmi2GetString"));
    fmu.setReal = reinterpret_cast<fmi2SetRealTYPE *>(sharedLibrary.resolve("fmi2SetReal"));
    fmu.setInteger = reinterpret_cast<fmi2SetIntegerTYPE *>(sharedLibrary.resolve("fmi2SetInteger"));
    fmu.setBoolean = reinterpret_cast<fmi2SetBooleanTYPE *>(sharedLibrary.resolve("fmi2SetBoolean"));
    fmu.setString = reinterpret_cast<fmi2SetStringTYPE *>(sharedLibrary.resolve("fmi2SetString"));
    fmu.getFMUstate = reinterpret_cast<fmi2GetFMUstateTYPE *>(sharedLibrary.resolve("fmi2GetFMUstate"));
    fmu.setFMUstate = reinterpret_cast<fmi2SetFMUstateTYPE *>(sharedLibrary.resolve("fmi2SetFMUstate"));
    fmu.freeFMUstate = reinterpret_cast<fmi2FreeFMUstateTYPE *>(sharedLibrary.resolve("fmi2FreeFMUstate"));
    fmu.serializedFMUstateSize = reinterpret_cast<fmi2SerializedFMUstateSizeTYPE *>(sharedLibrary.resolve("fmi2SerializedFMUstateSize"));
    fmu.serializeFMUstate = reinterpret_cast<fmi2SerializeFMUstateTYPE *>(sharedLibrary.resolve("fmi2SerializeFMUstate"));
    fmu.deSerializeFMUstate = reinterpret_cast<fmi2DeSerializeFMUstateTYPE *>(sharedLibrary.resolve("fmi2DeSerializeFMUstate"));
    fmu.getDirectionalDerivative = reinterpret_cast<fmi2GetDirectionalDerivativeTYPE *>(sharedLibrary.resolve("fmi2GetDirectionalDerivative"));
#ifdef FMI_COSIMULATION
    fmu.setRealInputDerivatives = reinterpret_cast<fmi2SetRealInputDerivativesTYPE *>(sharedLibrary.resolve("fmi2SetRealInputDerivatives"));
    fmu.getRealOutputDerivatives = reinterpret_cast<fmi2GetRealOutputDerivativesTYPE *>(sharedLibrary.resolve("fmi2GetRealOutputDerivatives"));
    fmu.doStep = reinterpret_cast<fmi2DoStepTYPE *>(sharedLibrary.resolve("fmi2DoStep"));
    fmu.cancelStep = reinterpret_cast<fmi2CancelStepTYPE *>(sharedLibrary.resolve("fmi2CancelStep"));
    fmu.getStatus = reinterpret_cast<fmi2GetStatusTYPE *>(sharedLibrary.resolve("fmi2GetStatus"));
    fmu.getRealStatus = reinterpret_cast<fmi2GetRealStatusTYPE *>(sharedLibrary.resolve("fmi2GetRealStatus"));
    fmu.getIntegerStatus = reinterpret_cast<fmi2GetIntegerStatusTYPE *>(sharedLibrary.resolve("fmi2GetIntegerStatus"));
    fmu.getBooleanStatus = reinterpret_cast<fmi2GetBooleanStatusTYPE *>(sharedLibrary.resolve("fmi2GetBooleanStatus"));
    fmu.getStringStatus = reinterpret_cast<fmi2GetStringStatusTYPE *>(sharedLibrary.resolve("fmi2GetStringStatus"));
#else // FMI for Model Exchange
    fmu.enterEventMode = reinterpret_cast<fmi2EnterEventModeTYPE *>(sharedLibrary.resolve("fmi2EnterEventMode"));
    fmu.newDiscreteStates = reinterpret_cast<fmi2NewDiscreteStatesTYPE *>(sharedLibrary.resolve("fmi2NewDiscreteStates"));
    fmu.enterContinuousTimeMode = reinterpret_cast<fmi2EnterContinuousTimeModeTYPE *>(sharedLibrary.resolve("fmi2EnterContinuousTimeMode"));
    fmu.completedIntegratorStep = reinterpret_cast<fmi2CompletedIntegratorStepTYPE *>(sharedLibrary.resolve("fmi2CompletedIntegratorStep"));
    fmu.setTime = reinterpret_cast<fmi2SetTimeTYPE *>(sharedLibrary.resolve("fmi2SetTime"));
    fmu.setContinuousStates = reinterpret_cast<fmi2SetContinuousStatesTYPE *>(sharedLibrary.resolve("fmi2SetContinuousStates"));
    fmu.getDerivatives = reinterpret_cast<fmi2GetDerivativesTYPE *>(sharedLibrary.resolve("fmi2GetDerivatives"));
    fmu.getEventIndicators = reinterpret_cast<fmi2GetEventIndicatorsTYPE *>(sharedLibrary.resolve("fmi2GetEventIndicators"));
    fmu.getContinuousStates = reinterpret_cast<fmi2GetContinuousStatesTYPE *>(sharedLibrary.resolve("fmi2GetContinuousStates"));
    fmu.getNominalsOfContinuousStates = reinterpret_cast<fmi2GetNominalsOfContinuousStatesTYPE *>(sharedLibrary.resolve("fmi2GetNominalsOfContinuousStates"));
#endif
    if (fmu.getVersion == NULL && fmu.instantiate == NULL) {
        printf("warning: Functions from FMI 2.0 could not be found in %s\n", dllPath.toStdWString().c_str());
        printf("warning: Simulator will look for FMI 2.0 RC1 functions names...\n");
        fmu.getTypesPlatform = reinterpret_cast<fmi2GetTypesPlatformTYPE *>(sharedLibrary.resolve("fmiGetTypesPlatform"));
        fmu.getVersion = reinterpret_cast<fmi2GetVersionTYPE *>(sharedLibrary.resolve("fmiGetVersion"));
        fmu.setDebugLogging = reinterpret_cast<fmi2SetDebugLoggingTYPE *>(sharedLibrary.resolve("fmiSetDebugLogging"));
        fmu.instantiate = reinterpret_cast<fmi2InstantiateTYPE *>(sharedLibrary.resolve("fmiInstantiate"));
        fmu.freeInstance = reinterpret_cast<fmi2FreeInstanceTYPE *>(sharedLibrary.resolve("fmiFreeInstance"));
        fmu.setupExperiment = reinterpret_cast<fmi2SetupExperimentTYPE *>(sharedLibrary.resolve("fmiSetupExperiment"));
        fmu.enterInitializationMode = reinterpret_cast<fmi2EnterInitializationModeTYPE *>(sharedLibrary.resolve("fmiEnterInitializationMode"));
        fmu.exitInitializationMode = reinterpret_cast<fmi2ExitInitializationModeTYPE *>(sharedLibrary.resolve("fmiExitInitializationMode"));
        fmu.terminate = reinterpret_cast<fmi2TerminateTYPE *>(sharedLibrary.resolve("fmiTerminate"));
        fmu.reset = reinterpret_cast<fmi2ResetTYPE *>(sharedLibrary.resolve("fmiReset"));
        fmu.getReal = reinterpret_cast<fmi2GetRealTYPE *>(sharedLibrary.resolve("fmiGetReal"));
        fmu.getInteger = reinterpret_cast<fmi2GetIntegerTYPE *>(sharedLibrary.resolve("fmiGetInteger"));
        fmu.getBoolean = reinterpret_cast<fmi2GetBooleanTYPE *>(sharedLibrary.resolve("fmiGetBoolean"));
        fmu.getString = reinterpret_cast<fmi2GetStringTYPE *>(sharedLibrary.resolve("fmiGetString"));
        fmu.setReal = reinterpret_cast<fmi2SetRealTYPE *>(sharedLibrary.resolve("fmiSetReal"));
        fmu.setInteger = reinterpret_cast<fmi2SetIntegerTYPE *>(sharedLibrary.resolve("fmiSetInteger"));
        fmu.setBoolean = reinterpret_cast<fmi2SetBooleanTYPE *>(sharedLibrary.resolve("fmiSetBoolean"));
        fmu.setString = reinterpret_cast<fmi2SetStringTYPE *>(sharedLibrary.resolve("fmiSetString"));
        fmu.getFMUstate = reinterpret_cast<fmi2GetFMUstateTYPE *>(sharedLibrary.resolve("fmiGetFMUstate"));
        fmu.setFMUstate = reinterpret_cast<fmi2SetFMUstateTYPE *>(sharedLibrary.resolve("fmiSetFMUstate"));
        fmu.freeFMUstate = reinterpret_cast<fmi2FreeFMUstateTYPE *>(sharedLibrary.resolve("fmiFreeFMUstate"));
        fmu.serializedFMUstateSize = reinterpret_cast<fmi2SerializedFMUstateSizeTYPE *>(sharedLibrary.resolve("fmiSerializedFMUstateSize"));
        fmu.serializeFMUstate = reinterpret_cast<fmi2SerializeFMUstateTYPE *>(sharedLibrary.resolve("fmiSerializeFMUstate"));
        fmu.deSerializeFMUstate = reinterpret_cast<fmi2DeSerializeFMUstateTYPE *>(sharedLibrary.resolve("fmiDeSerializeFMUstate"));
        fmu.getDirectionalDerivative = reinterpret_cast<fmi2GetDirectionalDerivativeTYPE *>(sharedLibrary.resolve("fmiGetDirectionalDerivative"));
#ifdef FMI_COSIMULATION
        fmu.setRealInputDerivatives = reinterpret_cast<fmi2SetRealInputDerivativesTYPE *>(sharedLibrary.resolve("fmiSetRealInputDerivatives"));
        fmu.getRealOutputDerivatives = reinterpret_cast<fmi2GetRealOutputDerivativesTYPE *>(sharedLibrary.resolve("fmiGetRealOutputDerivatives"));
        fmu.doStep = reinterpret_cast<fmi2DoStepTYPE *>(sharedLibrary.resolve("fmiDoStep"));
        fmu.cancelStep = reinterpret_cast<fmi2CancelStepTYPE *>(sharedLibrary.resolve("fmiCancelStep"));
        fmu.getStatus = reinterpret_cast<fmi2GetStatusTYPE *>(sharedLibrary.resolve("fmiGetStatus"));
        fmu.getRealStatus = reinterpret_cast<fmi2GetRealStatusTYPE *>(sharedLibrary.resolve("fmiGetRealStatus"));
        fmu.getIntegerStatus = reinterpret_cast<fmi2GetIntegerStatusTYPE *>(sharedLibrary.resolve("fmiGetIntegerStatus"));
        fmu.getBooleanStatus = reinterpret_cast<fmi2GetBooleanStatusTYPE *>(sharedLibrary.resolve("fmiGetBooleanStatus"));
        fmu.getStringStatus = reinterpret_cast<fmi2GetStringStatusTYPE *>(sharedLibrary.resolve("fmiGetStringStatus"));
#else // FMI2 for Model Exchange
        fmu.enterEventMode = reinterpret_cast<fmi2EnterEventModeTYPE *>(sharedLibrary.resolve("fmiEnterEventMode"));
        fmu.newDiscreteStates = reinterpret_cast<fmi2NewDiscreteStatesTYPE *>(sharedLibrary.resolve("fmiNewDiscreteStates"));
        fmu.enterContinuousTimeMode = reinterpret_cast<fmi2EnterContinuousTimeModeTYPE *>(sharedLibrary.resolve("fmiEnterContinuousTimeMode"));
        fmu.completedIntegratorStep = reinterpret_cast<fmi2CompletedIntegratorStepTYPE *>(sharedLibrary.resolve("fmiCompletedIntegratorStep"));
        fmu.setTime = reinterpret_cast<fmi2SetTimeTYPE *>(sharedLibrary.resolve("fmiSetTime"));
        fmu.setContinuousStates = reinterpret_cast<fmi2SetContinuousStatesTYPE *>(sharedLibrary.resolve("fmiSetContinuousStates"));
        fmu.getDerivatives = reinterpret_cast<fmi2GetDerivativesTYPE *>(sharedLibrary.resolve("fmiGetDerivatives"));
        fmu.getEventIndicators = reinterpret_cast<fmi2GetEventIndicatorsTYPE *>(sharedLibrary.resolve("fmiGetEventIndicators"));
        fmu.getContinuousStates = reinterpret_cast<fmi2GetContinuousStatesTYPE *>(sharedLibrary.resolve("fmiGetContinuousStates"));
        fmu.getNominalsOfContinuousStates = reinterpret_cast<fmi2GetNominalsOfContinuousStatesTYPE *>(sharedLibrary.resolve("fmiGetNominalsOfContinuousStates"));
#endif
    }
    return s;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FmuWrapper::printModelDescription(ModelDescription* md)
{
    Element* elem = (Element*)md;
    int i;
    int n; // number of attributes
    const char **attributes = getAttributesAsArray(elem, &n);
    Component *component;

    qInfo() << "\t\t" << getElementTypeName(elem);
    for (i = 0; i < n; i += 2)
    {
        qInfo() << "\t\t\t" << attributes[i] << attributes[i+1];
    }
    if (attributes) free(attributes);

#ifdef FMI_COSIMULATION
    component = getCoSimulation(md);
    if (!component)
    {
        qDebug() << "error: No CoSimulation element found in model description. This FMU is not for Co-Simulation.\n";
    }
#else // FMI_MODEL_EXCHANGE
    component = getModelExchange(md);
    if (!component)
    {
        qDebug() << "error: No ModelExchange element found in model description. This FMU is not for Model Exchange.\n";
    }
#endif
    qInfo() << "\t\t" << getElementTypeName((Element *)component);
    attributes = getAttributesAsArray((Element *)component, &n);
    for (i = 0; i < n; i += 2)
    {
        qInfo() << "\t\t\t" <<  attributes[i] << " = " << attributes[i+1];
    }
    if (attributes) free(attributes);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Registration of FMU interfaces
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FmuWrapper::defineFmiVariables()
{    
    int n = getScalarVariableSize(fmu.modelDescription);
    for (int k = 0; k < n; k++)
    {
        ScalarVariable* sv      = getScalarVariable(fmu.modelDescription, k);
        fmi2ValueReference vr    = getValueReference(sv);
        FmuVariable* var        = new FmuVariable();
        var->name               = getAttributeValue(reinterpret_cast<Element *>(sv), att_name);
        var->type               = getElementType(getTypeSpec(sv));
        var->causality          = getAttributeValue(reinterpret_cast<Element *>(sv), att_causality);
        var->variability        = getAttributeValue(reinterpret_cast<Element *>(sv), att_variability);
        var->unit               = getAttributeFromUnitOrDeclaredUnit(fmu.modelDescription,sv);
        var->description        = getAttributeValue(reinterpret_cast<Element *>(sv), att_description);
        var->reference          = vr;
        var->isState            = false;

        switch (getCausality(sv))
        {
        case enu_parameter :if(config()["prefixinput"]=="true" && var->variability=="tunable")
                var->name=name()+"."+var->name;// prefixed
            else if(config()["prefixoutput"]=="true" && var->variability=="fixed")
                var->name=name()+"."+var->name;// prefixed

            yParameters.append(var);

            break;
        case enu_input :    if(config()["prefixinput"]=="true")
                var->name=name()+"."+var->name;// prefixed
            yInputs.append(var);
            var->isState=true;
            break;
        case enu_output :   if(config()["prefixoutput"]=="true")
                var->name=name()+"."+var->name;// prefixed
            yOutputs.append(var);
            var->isState=true;
            break;
        }
        // Currently noting to do with elm_Enumeration, elm_String

        // attribute "start" means state variable
        if (getAttributeValue(getTypeSpec(sv),att_start))
        {
            QString str = getAttributeValue(getTypeSpec(sv),att_start);
            // register as state
            yStates.append(var);
            var->isState=true;
            switch (var->type)
            {
            case elm_Real:
                var->value.mReal=str.toFloat();
                break;
            case elm_Boolean:
                /// according to FMI2.0 specification,  Legal literals are false, true, 0, 1
                var->value.mInteger = (str=="true" || str=="1") ? 1 : 0;
                break;
            case elm_Integer:
                var->value.mInteger=str.toInt();
                break;
            default : break;
            }
        }
#if 0
        fmi2Real internalValue;// internal value
        fmu.getReal(component, &(var->reference), 1, &internalValue);

        qDebug()<< var->name<< var->type
                << "Value:"<< internalValue
                << "/" << getAttributeValue(getTypeSpec(sv),att_start)
                << "causality:"<< var->causality
                << "variability:"<< var->variability
                << (var->isState?"State":"") ;
#endif
    }
} // defineFmiVariables()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Declaration of FMU interfaces
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FmuWrapper::registerFmiVariable()
{
    defineFmiVariables();

    QList<FmuVariable*>::iterator iter;
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // register input FMU interface
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (iter = yInputs.begin(); iter != yInputs.end(); ++iter)
    {
        FmuVariable* var = (*iter);

        switch (var->type)
        {
        case elm_Real:
            fmi2Real vRealValue;// initial value in plublic data pool
            fmu.getReal(component, &(var->reference), 1, &vRealValue);
            var->value.mReal=vRealValue;// initial value in plublic data pool
            subscribe(var->name,
                      &(var->value.mReal),
                      var->unit,
                      var->description);
            break;
        case elm_Boolean:
            fmi2Boolean vBoolValue;// initial value in plublic data pool
            fmu.getBoolean(component,&(var->reference), 1, &vBoolValue);
            var->value.mInteger=vBoolValue;// initial value in plublic data pool
            subscribe(var->name,
                      &(var->value.mInteger),
                      var->unit,
                      var->description);
            break;
        case elm_Integer:
            fmi2Integer vIntValue;// initial value in plublic data pool
            fmu.getInteger(component,&(var->reference), 1, &vIntValue);
            var->value.mInteger=vIntValue;// initial value in plublic data pool
            subscribe(var->name,
                      &(var->value.mInteger),
                      var->unit,
                      var->description);
            break;
        default : break;
        }
    }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // register output FMU interface
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (iter = yOutputs.begin(); iter != yOutputs.end(); ++iter)
    {
        FmuVariable* var = (*iter);

        switch (var->type)
        {
        case elm_Real:publish(var->name,
                              &(var->value.mReal),
                              var->unit,
                              var->description);
            break;
        case elm_Boolean:publish(var->name,
                                 &(var->value.mInteger),
                                 var->unit,
                                 var->description);
            break;
        case elm_Integer:publish(var->name,
                                 &(var->value.mInteger),
                                 var->unit,
                                 var->description);
            break;
        default : break;
        }
    }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // register FMU states
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (iter = yStates.begin(); iter != yStates.end(); ++iter)
    {
        FmuVariable* var = (*iter);
        switch (var->type)
        {
        case elm_Real:state(var->name,&(var->value.mReal));
            break;
        case elm_Boolean:state(var->name,&(var->value.mInteger));
            break;
        case elm_Integer:state(var->name,&(var->value.mInteger));
            break;
        default : break;
        }
    }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // register FMU parameters and constants
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    if (bPublishParam==true)
    {
        qCDebug(log) << "## begin exposing parameters to ZE: " << name() <<" ##";
        for (iter = yParameters.begin(); iter != yParameters.end(); ++iter)
        {
            FmuVariable* var = (*iter);
            switch (var->type)
            {
            case elm_Real:
                fmi2Real vRealValue;
                if (var->isState)
                {
                    // state value reinjection to fmu
//                    vRealValue=var->value.mReal;
//                    fmu.setReal(component, &(var->reference), 1, &vRealValue);
                    // done in pushFromModelDescriptor()
                }
                else
                {
                    // initial value in plublic data pool
                    fmu.getReal(component, &(var->reference), 1, &vRealValue);
                    var->value.mReal=vRealValue;// initial value in plublic data pool
                }
                if (var->variability=="tunable")
                {
                    subscribeAppType(    FbsfAppType::cParameter,
                                         var->name,
                                         &(var->value.mReal),
                                         var->unit,
                                         var->description);
                    yInputs.append(var);// bugFix
                    if (bSnapshotParam==true)
                        state(var->name,&(var->value.mReal));

                    qCDebug(log) << "parameters - real - tunable: " << QString(" #%1#").arg(var->name);
                }
                else if (var->variability=="fixed")
                {
                    publishAppType(  FbsfAppType::cConstant,
                                     var->name,
                                     &(var->value.mReal),
                                     var->unit,
                                     var->description);
                    yOutputs.append(var);// bugFix
                    qCDebug(log) << "parameters - real - fixed: " << QString(" #%1#").arg(var->name);
                }
                break;
            case elm_Boolean:
                if (var->isState)
                {
                    // state value reinjection to fmu
                    // fmu.setBoolean(component, &(var->reference), 1, &(var->value.mInteger));
                    // done in pushFromModelDescriptor()
                }
                else
                {
                    fmi2Boolean vBoolValue;// initial value in plublic data pool
                    fmu.getBoolean(component,&(var->reference), 1, &vBoolValue);
                    var->value.mInteger=vBoolValue;// initial value in plublic data pool
                }
                if (var->variability=="tunable")
                {
                    subscribeAppType(    FbsfAppType::cParameter,
                                         var->name,
                                         &(var->value.mInteger),
                                         var->unit,
                                         var->description);
                    yInputs.append(var);// bugFix

                    if (bSnapshotParam==true)
                        state(var->name,&(var->value.mInteger));

                    qCDebug(log) << "parameters - boolean - tunable: " << QString(" #%1#").arg(var->name);
                }
                else if (var->variability=="fixed")
                {
                    publishAppType(  FbsfAppType::cConstant,
                                     var->name,
                                     &(var->value.mInteger),
                                     var->unit,
                                     var->description);
                    yOutputs.append(var);

                    qCDebug(log) << "parameters - boolean - fixed: " << QString(" #%1#").arg(var->name);
                }
                break;
            case elm_Integer:
                if (var->isState)
                {
                    // state value reinjection to fmu
                    // fmu.setInteger(component, &(var->reference), 1, &(var->value.mInteger));
                    // done in pushFromModelDescriptor()
                }
                else
                {
                    fmi2Integer vIntValue;// initial value in plublic data pool
                    fmu.getInteger(component,&(var->reference), 1, &vIntValue);
                    var->value.mInteger=vIntValue;// initial value in plublic data pool
                }
                if (var->variability=="tunable")
                {
                    subscribeAppType(    FbsfAppType::cParameter,
                                         var->name,
                                         &(var->value.mInteger),
                                         var->unit,
                                         var->description);
                    yInputs.append(var);// bugFix

                    if (bSnapshotParam==true)
                        state(var->name,&(var->value.mInteger));

                    qCDebug(log) << "parameters - integer - tunable: " << QString(" #%1#").arg(var->name);
                }
                else if (var->variability=="fixed")
                {
                    publishAppType(  FbsfAppType::cConstant,
                                     var->name,
                                     &(var->value.mInteger),
                                     var->unit,
                                     var->description);
                    yOutputs.append(var);// bugFix

                    qCDebug(log) << "parameters - ineteger - fixed: " << QString(" #%1#").arg(var->name);
                }
                break;
            default : break;
            }
        }
        qCDebug(log) << "## end exposing parameters to ZE: " << name() <<" ##";
    }

    pushFromModelDescriptor();

    publish(name()+".Time",&time);
} // registerFmiVariable()

void FmuWrapper::pushFromModelDescriptor(){

    qCDebug(log) << "## begin pushing variables from ModelDescription.xml to *.dll (when discrepancy is detected, tolerance 1.e-5 for real value): " << name() <<" ##";
    QList<FmuVariable*>::iterator iter;
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // register FMU parameters and constants
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (iter = yParameters.begin(); iter != yParameters.end(); ++iter)
    {
        FmuVariable* var = (*iter);
        switch (var->type)
        {
        case elm_Real:
            if (var->isState)
            {
                // state value reinjection to fmu
                fmi2Real vRealValue;
                fmu.getReal(getComponent(), &(var->reference), 1, &vRealValue);
                if (qAbs(vRealValue - var->value.mReal)>1.e-5) {
                    qCDebug(log) << "parameters - real" << QString(" #%1# - val xml: #%2# - val dll: #%3# - diff: #%4#").arg(var->name).arg(var->value.mReal).arg(vRealValue).arg(var->value.mReal-vRealValue);
                    vRealValue=var->value.mReal;
                    fmu.setReal(getComponent(), &(var->reference), 1, &vRealValue);
                }
            }
            break;

        case elm_Boolean:
            if (var->isState)
            {
                // state value reinjection to fmu
                fmi2Boolean vBoolValue;
                fmu.getBoolean(getComponent(), &(var->reference), 1, &vBoolValue);
                if (vBoolValue != var->value.mInteger) {
                    qCDebug(log) << "parameters - bool" << QString(" #%1# - val xml: #%2# - val dll: #%3#").arg(var->name).arg(var->value.mInteger).arg(vBoolValue);
                    fmu.setBoolean(getComponent(), &(var->reference), 1, &(var->value.mInteger));
                }
            }
            break;
        case elm_Integer:
            if (var->isState)
            {
                // state value reinjection to fmu
                fmi2Integer vIntValue;
                fmu.getInteger(getComponent(), &(var->reference), 1, &vIntValue);
                if (vIntValue != var->value.mInteger) {
                    qCDebug(log) << "parameters - int" << QString(" #%1# - val xml: #%2# - val dll: #%3#").arg(var->name).arg(var->value.mInteger).arg(vIntValue);
                    fmu.setInteger(getComponent(), &(var->reference), 1, &(var->value.mInteger));
                }
            }
            break;
        default : break;
        }
    }

    qCDebug(log) << "## end pushing variables from ModelDescription.xml to *.dll: " << name() <<" ##";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Allocation and declaration of Public data
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FmuWrapper::outputToCsv()
{
    int k;
    fmi2Real r;
    fmi2Integer i;
    fmi2Boolean b;
    fmi2String s;
    fmi2ValueReference vr;
    char sep = ';';
    int n = getScalarVariableSize(md);

    // print header row
    if (csvHeader)
    {
        // open result file
        if (!file.open(QFile::WriteOnly|QFile::Truncate))
        {
            qDebug () << "could not write to " << name()+".csv";
            csvStatus = false;
            return 0; // failure
        }
        {
            QTextStream stream(&file);
            stream << "time";
            csvHeader=false;
            for (k = 0; k < n; k++)
            {
                ScalarVariable* sv = getScalarVariable(md, k);
                stream << sep << getAttributeValue((Element *)sv, att_name);
            }
            stream << "\n";
        }
        outputToCsv();// output solution for time t0
    }
    else
    {
        QTextStream stream(&file);
        stream << time;
        for (k = 0; k < n; k++)
        {
            ScalarVariable* sv = getScalarVariable(md, k);
            // output values
            vr = getValueReference(sv);
            switch (getElementType(getTypeSpec(sv)))
            {
            case elm_Real:
                fmu.getReal(component, &vr, 1, &r);
                stream << sep << r;
                break;
            case elm_Integer:
            case elm_Enumeration:
                fmu.getInteger(component, &vr, 1, &i);
                stream << sep << i;
                break;
            case elm_Boolean:
                fmu.getBoolean(component, &vr, 1, &b);
                stream << sep << b;
                break;
            case elm_String:
                fmu.getString(component, &vr, 1, &s);
                stream << sep << s;
                break;
            default:
                stream << sep <<"NoValueForType=" << getElementType(getTypeSpec(sv));
            }
        }
        stream << "\n";
    }
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const char* FmuWrapper::fmi2StatusToString(fmi2Status status)
{
    switch (status){
    case fmi2OK:      return "ok";
    case fmi2Warning: return "warning";
    case fmi2Discard: return "discard";
    case fmi2Error:   return "error";
    case fmi2Fatal:   return "fatal";
#ifdef FMI_COSIMULATION
    case fmi2Pending: return "fmiPending";
#endif
    default:         return "?";
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// search a fmu for the given variable, matching the type specified.
// return NULL if not found
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ScalarVariable* FmuWrapper::getSV(FMU *fmu, char type, fmi2ValueReference vr)
{
    int i;
    int n = getScalarVariableSize(fmu->modelDescription);
    Elm tp;

    switch (type) {
    case 'r': tp = elm_Real;    break;
    case 'i': tp = elm_Integer; break;
    case 'b': tp = elm_Boolean; break;
    case 's': tp = elm_String;  break;
    default : tp = elm_BAD_DEFINED;
    }
    for (i = 0; i < n; i++) {
        ScalarVariable* sv = getScalarVariable(fmu->modelDescription ,i);
        if (vr == getValueReference(sv) && tp == getElementType(getTypeSpec(sv))) {
            return sv;
        }
    }
    return NULL;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// replace e.g. #r1365# by variable name and ## by # in message
// copies the result to buffer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FmuWrapper::replaceRefsInMessage(FMU *fmu, const char* msg, char* buffer, int nBuffer)
{
    int i = 0; // position in msg
    int k = 0; // position in buffer
    int n;
    char c = msg[i];
    while (c != '\0' && k < nBuffer) {
        if (c != '#') {
            buffer[k++] = c;
            i++;
            c = msg[i];
        } else {

            const char* end = strchr(msg + i + 1, '#');
            if (!end) {
                printf("unmatched '#' in '%s'\n", msg);
                buffer[k++] = '#';
                break;
            }
            n = end - (msg + i);
            if (n == 1) {
                // ## detected, output #
                buffer[k++] = '#';
                i += 2;
                c = msg[i];

            } else {
                char type = msg[i + 1]; // one of ribs
                fmi2ValueReference vr;
                int nvr = sscanf(msg + i + 2, "%u", &vr);
                if (nvr == 1) {
                    // vr of type detected, e.g. #r12#
                    ScalarVariable* sv = getSV(fmu,type, vr);
                    const char* name = sv ? getAttributeValue((Element *)sv, att_name) : "?";
                    sprintf(buffer + k, "%s", name);
                    k += strlen(name);
                    i += (n+1);
                    c = msg[i];

                } else {
                    // could not parse the number
                    printf("illegal value reference at position %d in '%s'\n", i + 2, msg);
                    buffer[k++] = '#';
                    break;
                }
            }
        }
    } // while
    buffer[k] = '\0';
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define MAX_MSG_SIZE 1000

void FmuWrapper::fmuLogger(void *componentEnvironment,
                           fmi2String instanceName, fmi2Status status,
                           fmi2String category, fmi2String message, ...)
{
    char msg[MAX_MSG_SIZE];
    va_list (argp);

    // replace C format strings
    va_start(argp, message);
    vsprintf(msg, message, argp);
#if 0 // static method don't work with fmu C++ instance member
    // replace e.g. ## and #r12#
    char* copy;
    copy = strdup(msg);
    replaceRefsInMessage(fmu,copy, msg, MAX_MSG_SIZE);
    free(copy);
#endif
    // print the final message
    if (!instanceName) instanceName = "?";
    if (!category) category = "?";
    printf("%s %s (%s): %s\n", fmi2StatusToString(status), instanceName, category, msg);
    va_end (argp);
}
