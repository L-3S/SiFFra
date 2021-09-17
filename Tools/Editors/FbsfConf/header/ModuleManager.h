#ifndef TESTCALL_H
#define TESTCALL_H


#include <QObject>
#include <QDir>
#include <iostream>
#include <QMap>
#include "NodeModule.h"
//#include "InfoModel.h"
#include <QVariant>

#include "TypeManager.h"


class ModuleManager : public QObject{
   Q_OBJECT
public:
    explicit ModuleManager (QObject* parent = 0) : QObject(parent) {
        InitModules();
    };

    Q_INVOKABLE void InitModules();

    NodeModule *GetNodeModuleNodeRec (NodeModule *tmp, int node);

    Q_INVOKABLE NodeModule *GetNodeModuleNode(int node);

    NodeModule *GetNodeModuleNodeParentRec (NodeModule *tmp, int node);


    Q_INVOKABLE std::vector<QString> getInfos(int idnode, int index);

    Q_INVOKABLE int getSizeInfos(int idnode);

    Q_INVOKABLE QString getInfosType(int idnode);

    Q_INVOKABLE void saveInfos(QString name, QString value);


    Q_INVOKABLE NodeModule *GetNodeModuleNodeParent(int node);

    Q_INVOKABLE int addChildModules();

    Q_INVOKABLE int addChildModules(int idnode, int idtype);

    Q_INVOKABLE int addChildModules(int idnode, TypeModule infos);

    Q_INVOKABLE int addChildModulesBetween(int idnode, bool isnode, int idtype);

    Q_INVOKABLE int addChildModulessesq(int idnode);

    Q_INVOKABLE int addChildModules(int idnode, bool isnode, QString name="noname");

    Q_INVOKABLE bool getNodeModuleIsNode(int idnode);

    Q_INVOKABLE int addChildModulesEndNode(int idnode);

    Q_INVOKABLE int addChildModulesEndNodeBetween(int idnode);

//    Q_INVOKABLE QVariantMap displayInfoNode(int idnode);

//    Q_INVOKABLE void saveMap(QVariantMap map, int idnode);

    Q_INVOKABLE NodeModule *getListModules();

    Q_INVOKABLE int getSizeListModules();

    Q_INVOKABLE void deleteNodeModule(int idnode);
    Q_INVOKABLE void deleteNodeModuleOnly(int idnode);
    Q_INVOKABLE void deleteNodeModuleAll();

    int getSpaceRec(NodeModule *tmp);

    Q_INVOKABLE int getSpace(int idnode);

    Q_INVOKABLE int getId() {return id;};

    Q_INVOKABLE QList<int> getNodeModule(int idnode);

    Q_INVOKABLE QList<int> getNodeModule();

    NodeModule *list_module = nullptr;
    TypeManager *managetype = nullptr;
    private:
        int id = 0;
        int currentnode = 0;

};


#endif // TESTCALL_H
