#include "FbsfBaseModelFMI.h"
#include "fmi2.h"
#include "XmlParserException.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FBSFBaseModelFMI::FBSFBaseModelFMI()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Destructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FBSFBaseModelFMI::~FBSFBaseModelFMI()
{
    freeModelDescription(mModelDescription);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// xml file parser
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FBSFBaseModelFMI::parseModelDescription(QString aFile)
{
    try {
        mModelDescription=parse(const_cast<char *>(aFile.toStdString().c_str()));
    }
    catch (XmlParserException ex)
    {
        qDebug() << "[Error] : error while parsing " << aFile;
        qDebug() << "\t\t" << ex.what();
        return FBSF_ERROR;// failure
    }
    if (!mModelDescription)
    {
        qDebug() << "[Error] : error while parsing " << aFile;
        return FBSF_ERROR;// failure
    }
    else
    {
        printModelDescription();
    }
    declareFmiParameters();
    return FBSF_OK;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// print the model description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModelFMI::printModelDescription()
{
    Element* elem = (Element*)mModelDescription;
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

    component = getCoSimulation(mModelDescription);
    if (!component)
    {
        qInfo() << "error: No CoSimulation element found in model description. This FMU is not for Co-Simulation.\n";
    }
    qInfo() << "\t\t" << getElementTypeName((Element *)component);
    attributes = getAttributesAsArray((Element *)component, &n);
    for (i = 0; i < n; i += 2)
    {
        qInfo() << "\t\t\t" <<  attributes[i] << " = " << attributes[i+1];
    }
    if (attributes) free(attributes);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Registration of FMI interfaces
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FBSFBaseModelFMI::declareFmiParameters()
{
    bool prefixinput=config()["prefixinput"]=="true"?true:false;
    bool prefixoutput=config()["prefixoutput"]=="true"?true:false;
    //parameter options
    //    bool bPublishParam =AppConfig()["publishparam"]=="true"?true:false;
    bool bSnapshotParam=AppConfig()["snapshotparam"]=="true"?true:false;

    int n = getScalarVariableSize(mModelDescription);
    for (int k = 0; k < n; k++)
    {
        ScalarVariable* sv      = getScalarVariable(mModelDescription, k);
        FmiVariable* var        = new FmiVariable();
        var->name(getAttributeValue(reinterpret_cast<Element *>(sv), att_name));
        var->causality(getAttributeValue(reinterpret_cast<Element *>(sv), att_causality));
        var->variability(getAttributeValue(reinterpret_cast<Element *>(sv), att_variability));
        var->unit(getAttributeFromUnitOrDeclaredUnit(mModelDescription,sv));
        var->description(getAttributeValue(reinterpret_cast<Element *>(sv), att_description));
        // Get start value if available
        Elm type = getElementType(getTypeSpec(sv));
        ValueStatus vs;
        switch (type)
        {
        case elm_Boolean:   {bool valBool=getAttributeBool(getTypeSpec(sv),att_start, &vs);
            if (vs==valueDefined) var->value(valBool); else var->value(false);
            break;}
        case elm_Integer:   {int valInt=getAttributeInt(getTypeSpec(sv),att_start, &vs);
            if (vs==valueDefined) var->value(valInt); else var->value(0);
            break;}
        case elm_Real:      {real valReal=getAttributeDouble(getTypeSpec(sv),att_start, &vs);
            if (vs==valueDefined) var->value(valReal); else var->value(0);
            break;}
        default :           break;
        }
        // Get causality : currently restricted to parameters
        switch (getCausality(sv))
        {
        case enu_parameter :
            if(var->variability()=="tunable")
            {
                if (prefixinput) var->name()=name()+"."+var->name();// prefixed
                {
                    switch (type)
                    {
                    case elm_Real:
                        subscribeAppType(   FbsfAppType::cParameter,
                                            var->name(),
                                            var->pRealValue(),
                                            var->unit(),
                                            var->description());
                        if (bSnapshotParam==true)
                            state(var->name(),var->pRealValue());
                        break;
                    case elm_Boolean:
                        subscribeAppType(   FbsfAppType::cParameter,
                                            var->name(),
                                            var->pIntValue(),
                                            var->unit(),
                                            var->description());
                        if (bSnapshotParam==true)
                            state(var->name(),var->pIntValue());
                        break;
                    case elm_Integer:
                        subscribeAppType(   FbsfAppType::cParameter,
                                            var->name(),
                                            var->pIntValue(),
                                            var->unit(),
                                            var->description());
                        if (bSnapshotParam==true)
                            state(var->name(),var->pIntValue());
                        break;
                    default : break;
                    }
                }
            }
            else if(var->variability()=="fixed")
            {
                if(prefixoutput) var->name()=name()+"."+var->name();// prefixed
                {
                    switch (type)
                    {
                    case elm_Real:
                        subscribeAppType(   FbsfAppType::cConstant,
                                            var->name(),
                                            var->pRealValue(),
                                            var->unit(),
                                            var->description());
                        if (bSnapshotParam==true)
                            state(var->name(),var->pRealValue());
                        break;
                    case elm_Boolean:
                        subscribeAppType(   FbsfAppType::cConstant,
                                            var->name(),
                                            var->pIntValue(),
                                            var->unit(),
                                            var->description());
                        if (bSnapshotParam==true)
                            state(var->name(),var->pIntValue());
                        break;
                    case elm_Integer:
                        subscribeAppType(   FbsfAppType::cConstant,
                                            var->name(),
                                            var->pIntValue(),
                                            var->unit(),
                                            var->description());
                        if (bSnapshotParam==true)
                            state(var->name(),var->pIntValue());
                        break;
                    default : break;
                    }
                }
            }
            yParameters[var->name()]=var;
            break;

        default:              break;
        }

#if 0
        DEBUG   << var->name()
                << "causality:"<< var->causality()
                << "variability:"<< var->variability();
#endif
    }
}
