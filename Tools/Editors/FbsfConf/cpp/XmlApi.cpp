#include "XmlApi.h"

void XmlApi::parseSimu(QXmlStreamReader *reader)
{
    QString s;
    while(reader->readNextStartElement()){
        if(reader->name() == "speedfactor") {
            s = reader->readElementText();
            speedfactor = s.toFloat();
            s.clear();
        }
        else if (reader->name() == "timestep"){
            s = reader->readElementText();
            timestep = s.toFloat();
            s.clear();
        }
        else
            reader->skipCurrentElement();
    }
}

XmlApi::plugin XmlApi::parsePlugin(QXmlStreamReader *reader)
{
    plugin tmp;
    QString s;
    while(reader->readNextStartElement()){
        if(reader->name() == "name") {
            s = reader->readElementText();
            tmp.name = s;
        }
        else if(reader->name() == "path") {
            s = reader->readElementText();
            tmp.path = s;
        }
        else
            reader->skipCurrentElement();
    }
    return tmp;
}

void XmlApi::parsePlugins(QXmlStreamReader *reader)
{
    QString s;
    while(reader->readNextStartElement()){
        if(reader->name() == "Plugin") {
            pluginlist.push_back(parsePlugin(reader));
        }
        else
            reader->skipCurrentElement();
    }
}

XmlApi::model XmlApi::parseModel(QXmlStreamReader *reader)
{
    model tmp;
    QString s;
    while(reader->readNextStartElement()){ // TODO ajouter des paramètre si ils sont pas déclaré dans la dll
        tmp.infosType.addParamsValue(reader->name().toString() ,reader->readElementText());
    }
    tmp.infosType.addManageType(spaceManager->managetype);
    tmp.infosType.checkLib();
    return tmp;
}

XmlApi::model XmlApi::parseSubSequence(QXmlStreamReader *reader)
{
    model tmp;
    QString s;
    tmp.sequence = true;
    while(reader->readNextStartElement()){
        if(reader->name() == "models") {
            std::cout << qPrintable("la") << std::endl;
            tmp.nodes = parseModels(reader);
        }
        else
            reader->skipCurrentElement();
    }
    return tmp;
}


std::vector<XmlApi::model> XmlApi::parseSubSequences(QXmlStreamReader *reader)
{
    std::vector<model> tmp;
    QString s;
    while(reader->readNextStartElement()){
        if (reader->name() == "SubSequence")
            tmp.push_back(parseSubSequence(reader));
        else
            reader->skipCurrentElement();
    }
    return tmp;
}


XmlApi::model XmlApi::parseNode(QXmlStreamReader *reader)
{
    model tmp;
    QString s;
    tmp.node = true;
    while(reader->readNextStartElement()){
        if(reader->name() == "name") {
            tmp.name = reader->readElementText();
        }
        else if (reader->name() == "SubSequences")
            tmp.nodes = parseSubSequences(reader);
        else
            reader->skipCurrentElement();
    }
    return tmp;
}


std::vector<XmlApi::model> XmlApi::parseModels(QXmlStreamReader *reader)
{
    QString s;
    std::vector<model> tmp;
    while(reader->readNextStartElement()){
        if(reader->name() == "model") {
            tmp.push_back(parseModel(reader));
        } /// LA
        else if (reader->name() == "node") {
            tmp.push_back(parseNode(reader));
        }
        else
            reader->skipCurrentElement();
    }
    return tmp;
}


XmlApi::sequence XmlApi::parseSequence(QXmlStreamReader *reader)
{
    sequence tmp;
    QString s;
    while(reader->readNextStartElement()){
        if(reader->name() == "name") {
            s = reader->readElementText();
            tmp.name = s;
        }
        else if(reader->name() == "iter") {
            s = reader->readElementText();
            tmp.iter = s.toInt();
        }
        else if(reader->name() == "period") {
            s = reader->readElementText();
            tmp.period = s.toInt();
        }
        else if(reader->name() == "models") {
            std::cout << qPrintable("la") << std::endl;
            tmp.models = parseModels(reader);
        }
        else
            reader->skipCurrentElement();
    }
    return tmp;

}



void XmlApi::parseSequences(QXmlStreamReader *reader)
{
    QString s;
    while(reader->readNextStartElement()){
        if(reader->name() == "sequence") {
            sequences.push_back(parseSequence(reader));
        }
        else
            reader->skipCurrentElement();
    }
}

///////////////////////////////////////////////////////////////////////////

int XmlApi::read()
{
    QFile file(to_read);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Cannot read file" << file.errorString();
        exit(0);
    }

    QXmlStreamReader reader(&file);




    if (reader.readNextStartElement()) {
            if (reader.name() == "Items"){
                while(reader.readNextStartElement()){

                    if (reader.name() == "simulation")
                        parseSimu(&reader);
                    if (reader.name() == "PluginsList")
                        parsePlugins(&reader);
                    if (reader.name() == "sequences")
                        parseSequences(&reader);
                }
            }
            else
                reader.raiseError(QObject::tr("Incorrect file"));
    }
    return 0;

}


void XmlApi::writeThis(QXmlStreamWriter *xmlWriter, QString value)
{
    xmlWriter->device()->write(value.toUtf8().constData(), value.toUtf8().length());
}

void XmlApi::writeThisWithThis(QXmlStreamWriter *xmlWriter, QString name, QString value)
{
    xmlWriter->writeStartElement(name);
    xmlWriter->writeCharacters("");
    writeThis(xmlWriter, value);
    xmlWriter->writeEndElement();
    xmlWriter->writeCharacters("\n");
}

void XmlApi::writeModel(QXmlStreamWriter *xmlWriter, model mode)
{
    xmlWriter->writeStartElement("model");
    xmlWriter->writeCharacters("");
    xmlWriter->writeCharacters("\n");
    QMap<QString,ParamProperties>::const_iterator iter = mode.infosType._params.constBegin();
    while (iter != mode.infosType._params.constEnd()) {
        writeThisWithThis(xmlWriter, iter.key() , mode.infosType._valueparams[iter.key()].toString()); // TODO
        iter++;
    }

//    for (auto e : mode.infosType._params) {
//        writeThisWithThis(xmlWriter, e , e.value);
//    }
    xmlWriter->writeEndElement();
    xmlWriter->writeCharacters("\n");

}

void XmlApi::writeNode(QXmlStreamWriter *xmlWriter, model node)
{
    xmlWriter->writeStartElement("node");
    xmlWriter->writeCharacters("");
    xmlWriter->writeCharacters("\n");
    writeThisWithThis(xmlWriter, "name", node.name);

    xmlWriter->writeStartElement("SubSequences");
    xmlWriter->writeCharacters("");
    xmlWriter->writeCharacters("\n");

    for (auto e : node.nodes)
    {
        if (e.sequence) {
            xmlWriter->writeStartElement("SubSequence");
            xmlWriter->writeCharacters("");
            xmlWriter->writeCharacters("\n");
            writeModels(xmlWriter, e.nodes);
            //
            xmlWriter->writeEndElement();
            xmlWriter->writeCharacters("\n");
        }
    }

    xmlWriter->writeEndElement();
    xmlWriter->writeCharacters("\n");

    xmlWriter->writeEndElement();
    xmlWriter->writeCharacters("\n");
}


void XmlApi::writeModels(QXmlStreamWriter *xmlWriter, std::vector<model> seq)
{
    xmlWriter->writeStartElement("models");
    xmlWriter->writeCharacters("");
    xmlWriter->writeCharacters("\n");

    for (auto a : seq)
    {
        if (a.node) {
            writeNode(xmlWriter, a);

        } else
            writeModel(xmlWriter, a);

    }

    xmlWriter->writeEndElement();
    xmlWriter->writeCharacters("\n");
}


void XmlApi::write()
{
     QFile file;
     file.setFileName(to_write);
     if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        qDebug("Create "" Success!");
    }
     QString xmlCode = "LOLO je suis le text";


     QXmlStreamWriter xmlWriter(&file);
//         xmlWriter.setAutoFormattingIndent(4);
//         xmlWriter.setAutoFormatting(true);

//         xmlWriter.writeCharacters("<?xml version='1.0' encoding='UTF-8' ?>");
//         xmlWriter.writeCharacters("\n");
     xmlWriter.writeStartElement("Items");
     xmlWriter.writeCharacters("");
     xmlWriter.writeCharacters("\n");

     xmlWriter.writeStartElement("simulation");
     xmlWriter.writeCharacters("");
     xmlWriter.writeCharacters("\n");

     writeThisWithThis(&xmlWriter, "timestep", QString::number(timestep));
     writeThisWithThis(&xmlWriter, "speedfactor", QString::number(speedfactor));

     xmlWriter.writeEndElement();
     xmlWriter.writeCharacters("\n");

     xmlWriter.writeStartElement("PluginsList");
     xmlWriter.writeCharacters("");
     xmlWriter.writeCharacters("\n");
     for (auto e : pluginlist)
     {
         xmlWriter.writeStartElement("Plugin");
         xmlWriter.writeCharacters("");
         xmlWriter.writeCharacters("\n");
         writeThisWithThis(&xmlWriter, "name", e.name);
         writeThisWithThis(&xmlWriter, "path", e.path);
         xmlWriter.writeEndElement();
         xmlWriter.writeCharacters("\n");
     }

     xmlWriter.writeEndElement();
     xmlWriter.writeCharacters("\n");


     xmlWriter.writeStartElement("sequences");
     xmlWriter.writeCharacters("");
     xmlWriter.writeCharacters("\n");

     for (auto e : sequences)
     {
         xmlWriter.writeStartElement("sequence");
         xmlWriter.writeCharacters("");
         xmlWriter.writeCharacters("\n");
         writeThisWithThis(&xmlWriter, "name", e.name);
         if (e.iter != 0)
             writeThisWithThis(&xmlWriter, "iter", QString::number(e.iter));
         else
             writeThisWithThis(&xmlWriter, "period", QString::number(e.period));

         writeModels(&xmlWriter, e.models);

         xmlWriter.writeEndElement();
         xmlWriter.writeCharacters("\n");
     }
     xmlWriter.writeEndElement();
     xmlWriter.writeCharacters("\n");

     xmlWriter.writeEndElement();
     xmlWriter.writeCharacters("\n");
}
