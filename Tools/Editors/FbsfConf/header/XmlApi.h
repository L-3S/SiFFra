#ifndef XMLAPI_H
#define XMLAPI_H

#include <QObject>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include <iostream>
#include <vector>
#include <limits>
#include "ModuleManager.h"
#include "TypeModule.h"


class XmlApi : public QObject
{
    Q_OBJECT

public:
    typedef struct plugin {
        QString path;
        QString name;
    } plugin_t;



    typedef struct model {
        // Node
        bool node = false;
        bool sequence = false;
        std::vector<model> nodes;
        QString name;
        TypeModule infosType;
    } model_t;


    typedef struct sequence {
        QString name;
        int iter = 0;
        int period = 0;
        std::vector<model> models;
    } sequence_t;

    explicit XmlApi (QObject* parent = 0) : QObject(parent) {
    };

    void parseSimu(QXmlStreamReader *reader);

    plugin parsePlugin(QXmlStreamReader *reader);

    void parsePlugins(QXmlStreamReader *reader);


    model parseModel(QXmlStreamReader *reader);

    model parseSubSequence(QXmlStreamReader *reader);

    std::vector<model> parseSubSequences(QXmlStreamReader *reader);


    model parseNode(QXmlStreamReader *reader);

    std::vector<model> parseModels(QXmlStreamReader *reader);

    sequence parseSequence(QXmlStreamReader *reader);


    void parseSequences(QXmlStreamReader *reader);


    int read();

    void writeThis(QXmlStreamWriter *xmlWriter, QString value);

    void writeThisWithThis(QXmlStreamWriter *xmlWriter, QString name, QString value);

    void writeModel(QXmlStreamWriter *xmlWriter, model mode);

    void writeNode(QXmlStreamWriter *xmlWriter, model node);


    void writeModels(QXmlStreamWriter *xmlWriter, std::vector<model> seq);

    Q_INVOKABLE void write();


    void defil_models(std::vector<model> models, int id, ModuleManager *manager)
    {
        int tmpid = 0;
//        bool inNode = false;
        for (auto a : models) {
            if (a.node) {
                std::cout << qPrintable("enter in node") << std::endl;
                tmpid = manager->addChildModules(id, true, a.name);
//                inNode = true;
                for (auto i : a.nodes) {
                    if (i.sequence) {
                        defil_models(i.nodes, tmpid, manager);
//                        for (auto c : i.nodes) {
//                            std::cout << qPrintable("in node "  + c.name) << std::endl << std::endl;
//                        }
                    }
                }
                id = manager->addChildModulesEndNode(tmpid); ///////////// RECENT CHANGE
            } else
            {
//                if (inNode) {
//                    std::cout << qPrintable("add end NODE") << std::endl;
//                    id = manager->addChildModulesEndNode(id);
//                    inNode = false;
//                } else {
//                }
                id = manager->addChildModules(id, a.infosType); //////////////////// CHANGE HEERE
//                std::cout << qPrintable(a.name) << std::endl;
            }
        }
    }

    void defil(ModuleManager *manager)
    {
        int id = 0;
        int baseid = manager->addChildModules();

        for (auto e : sequences)
        {
            id = manager->addChildModulessesq(baseid);
            std::cout << qPrintable(e.name) << std::endl << std::endl;
            defil_models(e.models, id, manager);

        }
        std::cout << qPrintable(QString::number(manager->list_module->list_next.size())) << std::endl << std::endl;


    }

    std::vector<model> saveModels(NodeModule* node)
    {
        std::vector<model> dest;
        model newmodel;
        model newnodemodel;

        model newsequence;

        model nodeinnode;

        NodeModule* tmp = node;
        NodeModule* tmpend = nullptr;

        newsequence.sequence = true;
        std::cout << qPrintable("saveModels") << std::endl;

        while (1) {

            if (tmp->endNode && !tmp->list_next.isEmpty()) {
                std::cout << qPrintable("ici2") << std::endl;
                tmp = tmp->list_next[0];
            }
            else if (tmp->endNode) {
                std::cout << qPrintable("ici3") << std::endl;
//                for (auto q : dest) {
//                    std::cout << qPrintable("A") << q.nodes.size() << std::endl;
//                }
                std::cout << qPrintable("A ") << dest.size() << std::endl;
                return dest;
            }


            if (tmp->node == true) {
                std::cout << qPrintable("node OULOULOU") << std::endl;
//                tmp = tmp->list_next[0];
//                newmodel.name = tmp->name; //////////////////
                newmodel.name = tmp->name;
                newmodel.node = true;

                int x = 0;
                for (auto e : tmp->list_next) {
                    if (e->endNode == false) {
                        newnodemodel.name = e->name; ///////////////////
                        newnodemodel.infosType = e->infosType;
                        if (!e->list_next.isEmpty()) {
                            std::cout << qPrintable("OUIiiiiiiiiiiiiiii") << std::endl;
                            if (e->node)
                                newsequence.nodes = saveModels(e); //////////////
                            else
                                newsequence.nodes = saveModels(e->list_next[0]); //////////////
                        }
                        if (!e->node)
                            newsequence.nodes.insert(newsequence.nodes.begin() + 0, newnodemodel);
                        newmodel.nodes.push_back(newsequence);
                        newsequence.nodes.clear();
                        newnodemodel.node = false;
                    } else {
                        std::cout << qPrintable("END NODE") << std::endl;
                        tmpend = e;
                     }
                    x += 1;
                }
                dest.push_back(newmodel);

                std::cout << qPrintable( QString::number(newmodel.nodes.size())) << std::endl;
                newmodel.node = false;
                newmodel.nodes.clear();
                tmp = tmpend;
                if (tmp == nullptr)
                    break;
                std::cout << qPrintable( "ici") << std::endl;
            } else {
                newmodel.infosType = tmp->infosType;
                dest.push_back(newmodel);
                if (!tmp->list_next.isEmpty())
                    tmp = tmp->list_next[0];
                else
                    break;
            }

        }
        return dest;
    }


    sequence saveSequence(NodeModule* node)
    {
        sequence seq;

        seq.name = node->name;
        std::cout << qPrintable("ICI " + node->name) << std::endl;
        if (!node->list_next.isEmpty())
            seq.models = saveModels(node->list_next[0]);
        return seq;

    }



    void saveMachine(NodeModule* seq)
    {
        for (auto e : seq->list_next) {
            sequences.push_back(saveSequence(e));
        }

    }

    Q_INVOKABLE void save(ModuleManager *manager)
    {
//        pluginlist.clear();
        sequences.clear();

        int i = manager->list_module->list_next.size();
        for (auto e : manager->list_module->list_next) {
            if (i > 1) {
                std::cout << qPrintable("Only One Machine is supported for the moment") << std::endl;
                return;
            } else
                saveMachine(e);
        }
        write();
    }

    Q_INVOKABLE void load(QString path)
    {
        to_read = path;
        pluginlist.clear();
        sequences.clear();
//        spaceManager->deleteNodeModule(0);
        read();
//        std::cout << qPrintable("OUAUAUA") << std::endl;
        defil(spaceManager);
    }


    Q_INVOKABLE QString getloadedxml()
    {
        return pathloaded;
    }


    QString pathloaded = "";
    ModuleManager *spaceManager;
private:
    float speedfactor = 0.0;
    float timestep = 0.0;
    std::vector<plugin> pluginlist;
    std::vector<sequence> sequences;
    QString to_read = "node2.xml";
    QString to_write = "node2.xml";
};

#endif // XMLAPI_H
