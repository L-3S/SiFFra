#include "FbsfConfiguration.h"
#include <QDomDocument>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#ifndef MODE_BATCH
#include <QtWidgets/QMessageBox>
#endif
#include <QDebug>

#define FIX_NESTED
QMap <QString, QList<QPair<int,int>> > FbsfConfiguration::mModuleList;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \class FbsfConfiguration
/// \brief Implements parsing the configuration of a simulator
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfConfiguration::FbsfConfiguration()
{
}
//FbsfConfiguration::~FbsfConfiguration()
//{
//}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// parse xml configuration file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfConfiguration::parseXML(QString aFileName)
{
    QFile file(aFileName);
    // If we can't open it, let's show an error message
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg("Can't open the configuration file "
                    + aFileName + " : " + file.errorString());
#ifndef MODE_BATCH
        QMessageBox::critical(nullptr, "[Fatal]", msg.toStdString().c_str());
#endif
        qFatal(msg.toStdString().c_str());
        return -1;
    }
    // store the config name without extension
    //mConfigName=aFileName.left(aFileName.lastIndexOf("."));
    mConfigName=QFileInfo(aFileName).baseName();// GR

    QDomDocument doc("configuration");
    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(&file,&errorMsg, &errorLine, &errorColumn))
    {
        QString msg="Error while parsing the configuration file\n"+errorMsg+"\t";
        msg += QString("line : %1 column : %2").arg(errorLine).arg(errorColumn);
#ifndef MODE_BATCH
        QMessageBox::warning(nullptr, "[Fatal]", msg.toStdString().c_str());
#endif

        qDebug() <<"Error while parsing the configuration file\n"
               << errorMsg << "\t"
               << "line : " <<  errorLine
               << "column : " << errorColumn;
        return -1;
    }

    // Get the root element
    QDomElement docElem = doc.documentElement();

    // Get the simulation attributes
    // GR : fix bug use docElem.firstChildElement to
    //      deal with unordered tag
    QDomElement simulation = docElem.firstChildElement("simulation");
    parseSimulation(simulation);
    //    QDomElement childElem=docElem.firstChild().toElement();
    //    if (childElem.tagName() =="simulation")
    //    {
    //        parseSimulation(childElem);
    //    }
    //~~~~~ end fix ~~~~~~

    // parse the xml plugin nodes
    QDomElement pluginList = docElem.firstChildElement("PluginsList");
    parsePlugins(pluginList);
    // Get the sequence node's interested in
    QDomNodeList sequenceList = docElem.elementsByTagName("sequence");


    // DataFlowGraph
    // Set an adress to each main sequences
    // {0,0} , {1,0}, {2,0}, ...
    for(int iSeq = 0;iSeq < sequenceList.count(); iSeq++)
    {
        if (sequenceList.at(iSeq).isComment()) continue;
        QList<QPair<int,int>> seqAdress={{iSeq,0}};
        parseSequences(sequenceList.at(iSeq).toElement(), seqAdress);
    }
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// parse xml simulation part
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfConfiguration::parseSimulation(const QDomElement& elem)
{
    mSimulation["version"] = elem.attribute("version"); // get version
    //get all data for the element, by looping through all child elements
    QDomNode pEntries = elem.firstChild();
    while(!pEntries.isNull())
    {
        QDomElement peData = pEntries.toElement();
        mSimulation[peData.tagName()]  = peData.text();
        pEntries = pEntries.nextSibling();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// parse xml plugin list
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfConfiguration::parsePlugins(const QDomElement& docElem)
{
    // Get the plugin node's interested in
    QDomNodeList pluginList = docElem.elementsByTagName("Plugin");
    // Check each node one by one.
    for(int iPlug = 0;iPlug < pluginList.count(); iPlug++)
    {
        QMap<QString, QString> Items;

        const QDomElement& elem=pluginList.at(iPlug).toElement();
        if (elem.isComment()) continue;
        // Get all data for the element, by looping through all child elements
        QDomNode pEntries = elem.firstChild();
        while(!pEntries.isNull())
        {
            QDomElement peData = pEntries.toElement();
            QString tagName = peData.tagName();
            Items[tagName]    = peData.text();
            pEntries = pEntries.nextSibling();
        }
        mpluginList.Models().append(Items);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// parse xml model list
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfConfiguration::parseSequences(const QDomElement& elem, QList<QPair<int,int>> seqAdress)
{
    QMap<QString, QString> Items;
    FbsfConfigSequence sequence;
    // Get all data for the element, by looping through all child elements
    QDomNode pEntries = elem.firstChild();
    while(!pEntries.isNull())
    {
        QDomElement peData = pEntries.toElement();
        QString tagName = peData.tagName();

        QList<QPair<int,int>> modelAdress= seqAdress;
        QList<QPair<int,int>> nodeAdress= seqAdress;

        if (tagName == "models")
        {
            QDomNodeList nodeList = pEntries.childNodes();
            // .elementsByTagName("sequence");
            // Get the node's interested in, this time only caring about models
            for(int iMod = 0;iMod < nodeList.count(); iMod++)
            {
                QDomElement mpeData = nodeList.at(iMod).toElement();
                QString mtagName = mpeData.tagName();
                if (nodeList.at(iMod).isComment()){
                    continue;
                } else if (mtagName == "node") {
                    parseConfigNode(sequence, nodeList.at(iMod).toElement(),nodeAdress);
                } else {
                    parseModels(sequence, nodeList.at(iMod).toElement(),modelAdress);
                    nodeAdress = modelAdress;
                    modelAdress[modelAdress.size()-1].second ++;
                }
            }
        }
        else { Items[tagName] = peData.text();}
        pEntries = pEntries.nextSibling();
    }
    sequence.Descriptor(Items);
    mListSequences.append(sequence);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// parse xml model list
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfConfiguration::parseModels(FbsfConfigSequence& aSequence, const QDomElement &elem, const QList<QPair<int,int>> modelAdress)
{
    QMap<QString, QString> Items;
    Items["version"] = elem.attribute("version"); // Get version

    // Get all data for the element, by looping through all child elements
    QDomNode pEntries = elem.firstChild();
    while(!pEntries.isNull())
    {
        QDomElement peData = pEntries.toElement();
        QString tagName = peData.tagName();
        Items[tagName]    = peData.text();
        pEntries = pEntries.nextSibling();
    }
    aSequence.Models().append(Items);

    // DataFlowGraph
    // Store the module name and it adress
    FbsfConfiguration::mModuleList.insert(Items["name"],modelAdress);
}

void FbsfConfiguration::parseConfigNode(FbsfConfigSequence& aSequence,const QDomElement &elem, const QList<QPair<int,int>> nodeAdress)
{
    static long long n = 0;
    n++;
    QMap<QString, QString> Items;
    FbsfConfigNode node;
    // when nested nodes, the sequencesList list include descendant SubSequences
    // so the test sequencesList.size() != 1 failed
#ifndef FIX_NESTED
    QDomNodeList sequencesList = elem.elementsByTagName("SubSequences");
#else
    // fix bug with nested nodes, get only first level elements
    QList<QDomElement> sequencesList;
    QDomElement child = elem.firstChildElement("SubSequences");
    while(!child.isNull()) {
      sequencesList.append( child );
      child = child.nextSiblingElement("SubSequences");
    }
    // SubSequences tag must be unique in the node scope ??????
    //~~~~~~~~~~~~~~~~~~~ end fix ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
    if (sequencesList.size() != 1) {
        qInfo() << "Missing tag: SubSequences";
        return;
    }
    Items["type"] = "node";
    Items["name"] = "node" + QString::number(n);
    for (int j = 0; j < sequencesList.size(); j++) {
        // when nested nodes, the sequenceList list include descendant SubSequence
        // so the nested sequences are appended to the upper level.
#ifndef FIX_NESTED
        QDomNodeList sequenceList = sequencesList.at(j).toElement().elementsByTagName("SubSequence");
#else
        QList<QDomElement> sequenceList;
        QDomElement child = sequencesList.at(j).toElement().firstChildElement("SubSequence");
        while(!child.isNull()) {
          sequenceList.append( child );
          child = child.nextSiblingElement("SubSequence");
        }
#endif

        // DataFlowGraph
        // Set an adress to each SubSequence
        QList<QPair<int,int>> subSeqAdress=nodeAdress ;
        subSeqAdress.append({0,0}); // FirstSubSequence

        for(int iSeq = 0;iSeq < sequenceList.count(); iSeq++)
        {
            if (sequenceList.at(iSeq).isComment()) continue;
            node.parseNode(sequenceList.at(iSeq).toElement(),subSeqAdress);
            subSeqAdress[subSeqAdress.size()-1].first ++;
        }
    }
    aSequence.Nodes().append(node);
    aSequence.Models().append(Items);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Check config
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfConfiguration::CheckValidity(const QMap<QString,QString> &aModuleXml,
                                         const QMap<QString,ParamProperties> &aModuleFields)
{
    /// We loop on:
    /// - Module Fields
    ///
    /// In each case, we verify that mandatory cases are present and test their integrity
    /// When present, we test integrity of optionnal fields
    QMap<QString,ParamProperties>::const_iterator iter = aModuleFields.constBegin();
    while (iter != aModuleFields.constEnd()) {
        switch (iter->mP_qual) {
        case Param_quality::cMandatory:
            if (!aModuleXml.contains(iter.key())) {
                qCritical() << "The mandatory parameter (module section) " <<  iter.key() << " is missing.";
                MsgBoxCriticalError();
            } else {
                syst_verif(aModuleXml, iter);
            }
            break;
        case Param_quality::cOptional:
            if (aModuleXml.contains(iter.key()))
                syst_verif(aModuleXml, iter);
            break;
        }
        ++iter;
    }
    return QString("");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfConfiguration::verif_limite(const QMap<QString,ParamProperties>::const_iterator iter,
                                     QVariant value)
{
    if (iter->mMin_strict == "" || iter->mMin_strict == QVariant()) {
    } else if (iter->mMin_strict >= value) {
        qCritical() << "The field corresponding to parameter " << iter.key() <<" is too low";
        MsgBoxCriticalError();
    }
    if (iter->mMax_strict == "" || iter->mMax_strict == QVariant()) {
    } else if (iter->mMax_strict <= value) {
        qCritical() << "The field corresponding to parameter " << iter.key() <<" is too High";
        MsgBoxCriticalError();
    }

    if (iter->mMin_warn == "" || iter->mMin_warn == QVariant()) {
    } else if (iter->mMin_warn >= value) {
        qWarning() << "The field corresponding to parameter " << iter.key() <<" is too low";
    }
    if (iter->mMax_warn == "" || iter->mMax_warn == QVariant()) {
    } else if (iter->mMax_warn <= value) {
        qWarning() << "The field corresponding to parameter " << iter.key() <<" is too High";
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfConfiguration::syst_verif(const QMap<QString, QString> &aXml,
                     const QMap<QString,ParamProperties>::const_iterator iter)
{
    bool ok;
    QFileInfo aFileInfo;
    QDateTime aDate;
    QStringList aList;
    QVector<int> aListInt;


    if (aXml.count(iter.key())>1) { // the verification seems to be before
        qInfo() << "The Xml contains several definitions for parameter " << iter.key() << ". Only one will be considered. Please revise parametrisation";
    }

    switch (iter->mP_type) {
    case Param_type::cStr:
        break;
    case Param_type::cDbl:
        aXml.value(iter.key()).toDouble(&ok);
        if (!ok) {
            qCritical() << "The field corresponding to parameter " << iter.key() <<" cannot be converted into a double.";
            MsgBoxCriticalError();
        } else {
            verif_limite(iter, aXml.value(iter.key()).toDouble());
        }
        break;
    case Param_type::cInt:
        aXml.value(iter.key()).toInt(&ok);
        if (!ok) {
            qCritical() << "The field corresponding to parameter " << iter.key() <<" cannot be converted into an int.";
            MsgBoxCriticalError();
        } else {
            verif_limite(iter, aXml.value(iter.key()).toInt());
        }
        break;
    case Param_type::cBool:
        ok = (aXml.value(iter.key()) == QString("true") ||
              aXml.value(iter.key()) == QString("false"))
                ;
        if (!ok) {
            qCritical() << "The field corresponding to parameter " << iter.key() <<" cannot be converted into an boolean (true or false).";
            MsgBoxCriticalError();
        }
        break;
    case Param_type::cPath:
        /// here we only check if a file exists
        aFileInfo.setFile(QDir::currentPath(), aXml.value(iter.key()));
        ok = aFileInfo.exists();
        if (!ok) {
            qCritical() << "The field corresponding to parameter " << iter.key() <<" is not a valid path to a file: " << aFileInfo.path() <<".";
            MsgBoxCriticalError();
        }
        break;
    case Param_type::cDateAndTime:
        aDate = QDateTime::fromString(aXml.value(iter.key()),"yyyy MM dd HH:mm:ss");
        ok = aDate.isValid();
        if (!ok) {
            qCritical() << "The field corresponding to parameter " << iter.key() << " is not a valid Date&Time format. Please follow the yyyy MM dd HH:mm:ss format (e.g. 1910 06 16 12:11:00).";
            MsgBoxCriticalError();
        }
        break;
    case Param_type::cDateAndTimeUTC:
        aDate = QDateTime::fromString(aXml.value(iter.key()),Qt::ISODate);
        ok = aDate.isValid();
        if (!ok) {
            qCritical() << "The field corresponding to parameter " << iter.key() << " is not a valid Date&Time in UTC format. Please follow the yyyy-MM-ddTHH:mm:ss (e.g. 2017-07-24T15:46:29) pattern, or with a time-zone suffix (Z for UTC otherwise an offset as [+|-]HH:mm) where appropriate for combined dates and times (e.g. 2015-03-25T12:00:00Z).";
            MsgBoxCriticalError();
        }
        break;
    case Param_type::cEnumString:
        QStringList().swap(aList);

        foreach (QVariant str, iter->mEnumEntries){
            aList.append(str.toString());
        }
        if (!aList.contains(aXml.value(iter.key()))) {
            qCritical() << "The field corresponding to parameter " + iter.key() + " should be chosen among the following possibilities: " << "#" << aList.join(QString("#")) << "#";
            MsgBoxCriticalError();
        }
        break;
    case Param_type::cEnumInt:
        QVector<int>().swap(aListInt);
        QStringList().swap(aList);

        foreach (QVariant str, iter->mEnumEntries){
            aListInt.append(str.toInt());
            aList.append(str.toString());
        }
        aXml.value(iter.key()).toInt(&ok);
        if (!ok) {
            qCritical() << "The field corresponding to parameter " << iter.key() <<" cannot be converted into an int.";
            MsgBoxCriticalError();
        }
        else if (!aListInt.contains(aXml.value(iter.key()).toInt(&ok))) {
            qCritical() << "The field corresponding to parameter " + iter.key() + " should be chosen among the following possibilities: " << "#" << aList.join(QString("#")) << "#";
            MsgBoxCriticalError();
        }
        break;
    }
}

void FbsfConfiguration::MsgBoxCriticalError()
{
#ifndef MODE_BATCH
    QMessageBox::warning(nullptr, "Critical error", "A critical error as been detected in the parameters.\nPlease check the log file.");
#endif
    qFatal("aborting");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfConfigNode::parseNode(const QDomElement &elem, const QList<QPair<int,int>> nodeAdress)
{
    parseSequences(elem,nodeAdress);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Chaque module possède une adresse dans l'arbre
// l'adresse est constituée de couple(s) :
//      No d'ordre de sequence dans son niveau
//      No d'ordre du module dans la sequence
//
// pour indiquer l'imbrication on ajoute des couples d'adresse.
// chaque fork (FbsfNode) passe son adresse a ses sequences
// qui la passent a leur modules, qui ajoutent leur propre adresse.
//
// exemple de structure avec 2 niveaux de sous-sequences.
//
//                    |(0,2)(0,0)--(0,2)(0,1)----------------|
// (0,0)--(0,1)--(0,2)|                                      |-- (0,3)
//                    |                      |(0,2)(1,1)(0,0)|
//                    |(0,2)(1,0)--(0,2)(1,1)|               |
//                                           |(0,2)(1,1)(1,0)|
// (1,0)--(0,1)
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfConfiguration::getDataFlowEdgeStatus(const QString moduleFrom, const QString moduleTo, int level)
{
    QList<QPair<int,int>> adressFrom = FbsfConfiguration::mModuleList.value(moduleFrom);
    QList<QPair<int,int>> adressTo = FbsfConfiguration::mModuleList.value(moduleTo);

    if(adressFrom.isEmpty() || moduleTo.isEmpty() )return "error"; // wrong name
    if(moduleFrom==moduleTo) return "self_loop";                   // self loop
    // get the sequence number and module number from address
    int SeqNumFrom= adressFrom[level].first;
    int SeqNumTo=adressTo[level].first;
    int ModNumFrom=adressFrom[level].second;
    int ModNumTo=adressTo[level].second;
    // Check the sequence then module number
    if(SeqNumFrom == SeqNumTo)
    {   // same sequence
        if(ModNumFrom < ModNumTo)
            return "forward";
        else if (ModNumFrom > ModNumTo)
            return "backward";
        else    // means sub-level
            return getDataFlowEdgeStatus(moduleFrom,moduleTo,++level);
    }
    else
    {   // different sequence
        return "ambigous";
    }
}
