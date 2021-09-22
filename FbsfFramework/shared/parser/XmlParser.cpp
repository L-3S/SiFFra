//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// XmlParser.cpp
// Parser implementation for xml model description file of a FMI 2.0 model.
// The result of parsing is a ModelDescription object that can be queried to
// get necessary information.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "XmlParser.h"
#include <utility>
#include <vector>
#include "XmlElement.h"
#include "XmlParserException.h"

#ifdef STANDALONE_XML_PARSER
#define logThis(n, ...) printf(__VA_ARGS__); printf("\n")
#define checkStrdup(str) strdup(str)
#else
#include "GlobalIncludes.h"
#include "logging.h"  // logThis
#include "minutil.h"  // checkStrdup
#endif  // STANDALONE_XML_PARSER

/* Helper functions to check validity of xml. */
static int checkAttribute(const char* att);

const char *XmlParser::elmNames[SIZEOF_ELM] = {
    "fmiModelDescription", "ModelExchange", "CoSimulation", "SourceFiles", "File",
    "UnitDefinitions", "Unit", "BaseUnit", "DisplayUnit", "TypeDefinitions",
    "SimpleType", "Real", "Integer", "Boolean", "String",
    "Enumeration", "Item", "LogCategories", "Category", "DefaultExperiment",
    "VendorAnnotations", "Tool", "ModelVariables", "ScalarVariable", "Annotations",
    "ModelStructure", "Outputs", "Derivatives", "DiscreteStates", "InitialUnknowns",
    "Unknown"
};

const char *XmlParser::attNames[SIZEOF_ATT] = {
    "fmiVersion", "modelName", "guid", "description", "author",
    "version", "copyright", "license", "generationTool", "generationDateAndTime",
    "variableNamingConvention", "numberOfEventIndicators", "name", "kg", "m",
    "s", "A", "K", "mol", "cd",
    "rad", "factor", "offset", "quantity", "unit",
    "displayUnit", "relativeQuantity", "min", "max", "nominal",
    "unbounded", "value", "startTime", "stopTime", "tolerance",
    "stepSize", "valueReference", "causality", "variability", "initial",
    "previous", "canHandleMultipleSetPerTimeInstant", "declaredType", "start", "derivative",
    "reinit", "index", "dependencies", "dependenciesKind", "modelIdentifier",
    "needsExecutionTool", "completedIntegratorStepNotNeeded", "canBeInstantiatedOnlyOncePerProcess",
        "canNotUseMemoryManagementFunctions", "canGetAndSetFMUstate",
    "canSerializeFMUstate", "providesDirectionalDerivative", "canHandleVariableCommunicationStepSize",
        "canInterpolateInputs", "maxOutputDerivativeOrder",
    "canRunAsynchronuously",

    // not conform with the FMI 2.0 doc.  // TODO : remove when used models become mature.
    "xmlns:xsi",                       // Dymola examples from 2014 FD01
    "providesDirectionalDerivatives",  // Dymola examples from 2014 FD01
    "canHandleEvents"                  // Dymola examples from 2014 FD01
};

const char *XmlParser::enuNames[SIZEOF_ENU] = {
    "flat", "structured", "dependent", "constant", "fixed",
    "tunable", "discrete", "parameter", "calculatedParameter", "input",
    "output", "local", "independent", "continuous", "exact",
    "approx", "calculated"
};

XmlParser::XmlParser(char *xmlPath) {
    this->xmlPath = (char *)checkStrdup(xmlPath);
}

XmlParser::~XmlParser() {
    free(xmlPath);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModelDescription *XmlParser::parse()
{
    ModelDescription *md = nullptr;
    QFile file(xmlPath);
    // If we can't open it, let's show an error message
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg("Can't open the ModelDescription file "
                    + QString(xmlPath) + " : " + file.errorString());
        qFatal(msg.toStdString().c_str());
    }
    QDomDocument doc("ModelDescription");
    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(&file,&errorMsg, &errorLine, &errorColumn))
    {
        qDebug() <<"Error while parsing the ModelDescription file\n"
               << errorMsg << "\t"
               << "line : " <<  errorLine
               << "column : " << errorColumn;
    }
    // Get the root element
    QDomElement docElem = doc.documentElement();
    if (docElem.tagName() != elmNames[elm_fmiModelDescription])
    {
        throw XmlParserException("Expected '%s' element. Found instead: '%s'.",
            elmNames[elm_fmiModelDescription],
            docElem.tagName().toStdString().c_str());
    }
    md = new ModelDescription;
    md->type = elm_fmiModelDescription;
    parseElementAttributes(static_cast<Element *>(md),docElem);
    parseChildElements(md,docElem);
    //if (md) md->printElement(0);

    return validate(md);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void XmlParser::parseElementAttributes(Element *mdElem,const QDomElement& xmlElem)
{
    const QDomNamedNodeMap& attrMap = xmlElem.attributes();
    for(int i=0;i< attrMap.length();i++)
    {
        auto attr=attrMap.item(i).toAttr() ;
        XmlParser::Att key = checkAttribute(attr.name().toStdString().c_str());
        char *theValue = attr.value().toStdString().c_str() ?
                    (char *)checkStrdup(attr.value().toStdString().c_str()) : NULL;
        mdElem->attributes.insert(std::pair<XmlParser::Att, char *>(key, theValue ));
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void XmlParser::parseChildElements(Element *mdElem,const QDomElement& xmlElem)
{
    for(QDomNode n = xmlElem.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        if(n.nodeType() == QDomNode::ElementNode)
            mdElem->handleElement(this, n.nodeName().toStdString().c_str(),
                                  !n.hasChildNodes(),n.toElement());
    }
}


void XmlParser::parseEndElement() {

}

void XmlParser::parseSkipChildElement() {
}

bool XmlParser::readNextInXml() {
    return true;
}

/* -------------------------------------------------------------------------* 
 * Helper functions to check validity of xml.
 * -------------------------------------------------------------------------*/

// Returns the index of name in the array.
// Throw exception if name not found (invalid).
static int checkName(const char *name, const char *kind, const char *array[], int n) {
    for (int i = 0; i < n; i++) {
        if (!strcmp(name, array[i])) {
            return i;
        }
    }
    throw XmlParserException("Illegal %s %s", kind, name);
}

XmlParser::Att XmlParser::checkAttribute(const char *att) {
    return (XmlParser::Att)checkName(att, "attribute", XmlParser::attNames, XmlParser::SIZEOF_ATT);
}

XmlParser::Elm XmlParser::checkElement(const char *elm) {
    return (XmlParser::Elm)checkName(elm, "element", XmlParser::elmNames, XmlParser::SIZEOF_ELM);
}

XmlParser::Enu XmlParser::checkEnumValue(const char *enu) {
    return (XmlParser::Enu)checkName(enu, "enum value", XmlParser::enuNames, XmlParser::SIZEOF_ENU);
}

ModelDescription *XmlParser::validate(ModelDescription *md) {
    if (md == NULL) return NULL;
    int errors = 0;
    // check modelDescription required attributes
    if (!(md->getAttributeValue(XmlParser::att_fmiVersion)
        && md->getAttributeValue(XmlParser::att_modelName)
        && md->getAttributeValue(XmlParser::att_guid)))
    {
        qDebug() << ERROR_ERROR<<QString("Model description miss required attributes in file %1" ).arg(xmlPath );
        return NULL;
    }

    if (!(md->coSimulation || md->modelExchange))
    {
        qDebug() << ERROR_ERROR<<QString("Model description must have a co-simulation or model exchange component in file %1").arg( xmlPath );
        return NULL;
    }

    // check model variables
    for (std::vector<ScalarVariable *>::const_iterator it = md->modelVariables.begin(); it != md->modelVariables.end();
            ++it) {
        const char *varName = (*it)->getAttributeValue(XmlParser::att_name);
        if (!varName)
        {
            qDebug() << ERROR_ERROR
                     <<QString("Scalar variable miss required %1 attribute in modelDescription.xml")
                        .arg(XmlParser::attNames[XmlParser::att_name]);
            errors++;
            continue;
        }
        XmlParser::ValueStatus vs;
        (*it)->getAttributeUInt(XmlParser::att_valueReference, &vs);
        if (vs == XmlParser::valueMissing) {
            qDebug() << ERROR_ERROR
                     <<QString("Scalar variable %1 miss required %2 attribute in modelDescription.xml")
                        .arg(varName, XmlParser::attNames[XmlParser::att_valueReference]);
            errors++;
            continue;
        }
        if (vs == XmlParser::valueIllegal) {
            qDebug() << ERROR_ERROR
                     <<QString("Scalar variable %1 has illegal format for %2 attribute in modelDescription.xml")
                        .arg(varName, XmlParser::attNames[XmlParser::att_valueReference]);
            errors++;
            continue;
        }
        if (!(*it)->typeSpec) {
            qDebug() << ERROR_ERROR
                     <<QString("Scalar variable %1 (%2) miss type specification in modelDescription.xml")
                        .arg(varName, XmlParser::attNames[XmlParser::att_valueReference]);
            errors++;
            continue;
        }
        if ((*it)->typeSpec->type == XmlParser::elm_Enumeration) {
            const char *typeName = (*it)->typeSpec->getAttributeValue(XmlParser::att_declaredType);
            if (!typeName) {
                qDebug() << ERROR_ERROR
                         <<QString("Scalar variable %1 with enum type specification miss required %2 attribute in modelDescription.xml")
                            .arg(varName, XmlParser::attNames[XmlParser::att_declaredType]);
                errors++;
                continue;
            }
            if (!md->getSimpleType(typeName)) {
                qDebug() << ERROR_ERROR
                         <<QString("Declared type %1 of variable %2 not found in modelDescription.xml")
                            .arg(typeName, varName);
                errors++;
                continue;
            }
        }
    }

    if (errors > 0) {
        qDebug() << ERROR_ERROR << "Found" << errors <<  "error in file" << xmlPath;
        return NULL;
    }
    return md;
}

// #define TEST
#ifdef TEST
int main() {
    // { char c='c'; while(c!='g') scanf("%c", &c); } // to debug: wait for the g key

    XmlParser *parser =
        new XmlParser("c:\\_data\\fmi-fmu\\fmi-fmuVS\\fmi-fmuVS\\modelDescriptionDymola - ManuallyModifiedCopy.xml");
    ModelDescription *md = parser->parse();
    if (md) md->printElement(0);
    delete parser;
    if (md) delete md;

    dumpMemoryLeaks();
    return 0;
}
#endif  // TEST
