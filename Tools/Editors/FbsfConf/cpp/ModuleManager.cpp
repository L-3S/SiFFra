#include "ModuleManager.h"

Q_INVOKABLE void ModuleManager::InitModules()
{
    list_module = new NodeModule;
}

Q_INVOKABLE void ModuleManager::saveInfos(QString name, QString value)
{
    NodeModule *tmp = GetNodeModuleNode(currentnode); // can be optimized

    tmp->infosType._valueparams[name] = value;

//    for (auto &e : tmp->infosType._params) {
//        std::cout << qPrintable(e.name + " ?= " + name) << std::endl;
//        if (e.name == name) {
//            e.value = value;
//            break;
//        }
//    }
}

Q_INVOKABLE std::vector<QString> ModuleManager::getInfos(int idnode, int index)
{
    NodeModule *tmp = GetNodeModuleNode(idnode); // can be optimized
    currentnode = idnode;
    QStringList aList;
    QString listenum;



    if (index > tmp->infosType._valueparams.size() - 1) {
        return  std::vector<QString>{};
    }

//    std::cout << qPrintable(tmp->infosType._params[index].type) << std::endl;
//    std::cout << tmp->infosType._valueparams[(tmp->infosType._params.constBegin() + index).key()].toString().toStdString() << std::endl;

//    std::cout << tmp->infosType._params[(tmp->infosType._valueparams.constBegin() + index).key()].mP_type << std::endl;
    QString type = "U";

    ParamProperties param = tmp->infosType._params[(tmp->infosType._valueparams.constBegin() + index).key()];

//    std::cout << tmp->infosType._valueparams[(tmp->infosType._valueparams.constBegin() + index).key()].toString().toStdString() << std::endl;
    switch (param.mP_type) {
    case Param_type::cStr:
        type = "str";
        break;
    case Param_type::cDbl:
        type = "dbl";

        return (std::vector<QString> {
            type,
            (tmp->infosType._valueparams.constBegin() + index).key(),
            tmp->infosType._valueparams[(tmp->infosType._valueparams.constBegin() + index).key()].toString(),
                    param.mMin_strict.toString(),
                    param.mMax_strict.toString(),
                    param.mMin_warn.toString(),
                    param.mMax_warn.toString(),
        });
        break;
        break;
    case Param_type::cInt:
        type = "int";

        return (std::vector<QString> {
            type,
            (tmp->infosType._valueparams.constBegin() + index).key(),
            tmp->infosType._valueparams[(tmp->infosType._valueparams.constBegin() + index).key()].toString(),
                    param.mMin_strict.toString(),
                    param.mMax_strict.toString(),
                    param.mMin_warn.toString(),
                    param.mMax_warn.toString(),
        });
        break;
    case Param_type::cBool:
        type = "bool";
        break;
    case Param_type::cPath:
        type = "path";
        break;
    case Param_type::cDateAndTime:
        type = "date";
        break;
    case Param_type::cDateAndTimeUTC:
        type = "dateutc";
        break;
    case Param_type::cEnumString:
        type = "enumstring";
//        QStringList().swap(aList);
        foreach (QVariant str, param.mEnumEntries){
            aList.append(str.toString());
        }

        listenum = aList.join(QString("#"));
        return (std::vector<QString> {
            type,
            (tmp->infosType._valueparams.constBegin() + index).key(),
            tmp->infosType._valueparams[(tmp->infosType._valueparams.constBegin() + index).key()].toString(),
            listenum
        });
        break;

    case Param_type::cEnumInt:
        type = "enumint";
        foreach (QVariant str, param.mEnumEntries){
            aList.append(str.toString());
        }

        listenum = aList.join(QString("#"));
        return (std::vector<QString> {
            type,
            (tmp->infosType._valueparams.constBegin() + index).key(),
            tmp->infosType._valueparams[(tmp->infosType._valueparams.constBegin() + index).key()].toString(),
            listenum
        });
        break;
    }

//    tmp->infosType._params[(tmp->infosType._valueparams.constBegin() + index).key()].mP_type;

    std::cout << type.toStdString() << std::endl;

    return (std::vector<QString> {
        type,
        (tmp->infosType._valueparams.constBegin() + index).key(),
        tmp->infosType._valueparams[(tmp->infosType._valueparams.constBegin() + index).key()].toString()
    });


//    return (std::vector<QString> {
////        tmp->infosType._params[index],
//                "str",
//        (tmp->infosType._params.constBegin() + index).key(),
////                "2",
////                "3"
//                tmp->infosType._valueparams[(tmp->infosType._params.constBegin() + index).key()].toString()
////                tmp->infosType._valueparams[(tmp->infosType._params.constBegin() + index).key()]
////        tmp->infosType._params[index].name,
////        (tmp->infosType._valueparams.constBegin() + index).value().toString()
//    });
}


Q_INVOKABLE int ModuleManager::getSizeInfos(int idnode)
{
    NodeModule *tmp = GetNodeModuleNode(idnode); // can be optimized


    return tmp->infosType._valueparams.size();
}

Q_INVOKABLE QString ModuleManager::getInfosType(int idnode)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    if (tmp->infosType.getName() == "") {
        return tmp->infosType.getType();
    }

    return tmp->infosType.getName();
}


Q_INVOKABLE QVariantMap getMap(QMap<QString, QString> m_intMap) {
        QVariantMap rval;
        foreach (QString key, m_intMap.keys()) {
            // int has an implicit conversion to QVariant
            rval[key] = m_intMap[key];
        }
        return rval;
    }

NodeModule *ModuleManager::GetNodeModuleNodeRec (NodeModule *tmp, int node)
{
    NodeModule *rslt = nullptr;

    for (auto e : tmp->list_next) {
        if (e->id == node)
            return e;
        if (e->list_next.size() != 0) {
            rslt = GetNodeModuleNodeRec(e, node);
            if (rslt != nullptr)
                return rslt;
        }
    }
    return nullptr;
}

Q_INVOKABLE NodeModule *ModuleManager::GetNodeModuleNode(int node)
{
    NodeModule *tmp = list_module;
    NodeModule *rslt = nullptr;

    rslt = GetNodeModuleNodeRec(tmp, node);
    if (rslt == nullptr)
        std::cout << qPrintable("OULALALALALALAL") << std::endl;
    return rslt;
}

NodeModule *ModuleManager::GetNodeModuleNodeParentRec (NodeModule *tmp, int node)
{
    NodeModule *rslt = nullptr;

    for (auto e : tmp->list_next) {
        if (e->id == node)
            return tmp;
        if (e->list_next.size() != 0) {
            rslt = GetNodeModuleNodeParentRec(e, node);
            if (rslt != nullptr)
                return rslt;
        }
    }
    return nullptr;
}

Q_INVOKABLE NodeModule *ModuleManager::GetNodeModuleNodeParent(int node)
{
    NodeModule *tmp = list_module;
    NodeModule *rslt = nullptr;
    rslt = GetNodeModuleNodeParentRec(tmp, node);
    if (rslt == nullptr) {
        std::cout << qPrintable("OULALALALALALAL") << std::endl;
        return nullptr;
    }
    return rslt;
}


Q_INVOKABLE int ModuleManager::addChildModules()
{
    NodeModule *tmp = list_module;
    NodeModule *new_node = new NodeModule;

    tmp->list_next.push_back(new_node);
    new_node->id = id;
    id += 1;
    return new_node->id;
}
Q_INVOKABLE int ModuleManager::addChildModulessesq(int idnode)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    NodeModule *new_node = new NodeModule;


    new_node->name = "ima seq";
    new_node->sequence = true;
    new_node->id = id;
    tmp->list_next.push_back(new_node);
    id += 1;
    return new_node->id;
}

Q_INVOKABLE int ModuleManager::addChildModules(int idnode, int idtype)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    NodeModule *new_node = new NodeModule;

    std::cout << qPrintable("addChildModules") << std::endl;

    new_node->name = "default name";
    new_node->id = id;
    if (idtype > -1)
        new_node->infosType = managetype->getTypeByIndex(idtype);
    tmp->list_next.push_back(new_node);
    id += 1;
    return new_node->id;
}
Q_INVOKABLE int ModuleManager::addChildModules(int idnode, TypeModule infos)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    NodeModule *new_node = new NodeModule;

    std::cout << qPrintable("addChildModules") << std::endl;

    new_node->infosType = infos;
    new_node->id = id;
    tmp->list_next.push_back(new_node);
    id += 1;
    return new_node->id;
}

Q_INVOKABLE int ModuleManager::addChildModulesEndNode(int idnode)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    NodeModule *new_node = new NodeModule;

    std::cout << qPrintable("addChildModulesEndNode") << std::endl;

    new_node->id = id;
    new_node->name = "ima Endnode";
    new_node->endNode = true;
    tmp->list_next.push_back(new_node);
    id += 1;
    return new_node->id;
}

Q_INVOKABLE int ModuleManager::addChildModulesEndNodeBetween(int idnode)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    NodeModule *new_node = new NodeModule;

    std::cout << qPrintable("addChildModulesEndNode") << std::endl;

    new_node->id = id;
    new_node->name = "ima Endnode";
    new_node->endNode = true;
    new_node->list_next =  tmp->list_next;
    tmp->list_next.clear();
    tmp->list_next.push_back(new_node);
    id += 1;
    return new_node->id;
}


Q_INVOKABLE int ModuleManager::addChildModules(int idnode, bool isnode,  QString name)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    NodeModule *new_node = new NodeModule;

    std::cout << qPrintable("addChildModulesNODE") << std::endl;

    new_node->id = id;
    new_node->node = isnode;
    if (isnode)
        name = "NODE";
    new_node->name = name;
    tmp->list_next.push_back(new_node);
    id += 1;
    return new_node->id;
}

Q_INVOKABLE int ModuleManager::addChildModulesBetween(int idnode, bool isnode, int idtype)
{
    NodeModule *tmp = GetNodeModuleNode(idnode);
    NodeModule *new_node = new NodeModule;

    std::cout << qPrintable("addChildModules") << std::endl;

    if (idtype > -1)
        new_node->infosType = managetype->getTypeByIndex(idtype);
    new_node->name = "imanode";
    new_node->id = id;
    new_node->node = isnode;
    new_node->list_next = tmp->list_next;
    tmp->list_next.clear();
    tmp->list_next.push_back(new_node);
    id += 1;
    return new_node->id;
}

Q_INVOKABLE NodeModule *ModuleManager::getListModules()
{
    return list_module;
}

Q_INVOKABLE int ModuleManager::getSizeListModules()
{
    return list_module->list_next.last()->list_next.size();
}


Q_INVOKABLE void ModuleManager::deleteNodeModuleOnly(int idnode)
{
    NodeModule *node = GetNodeModuleNodeParent(idnode);
    int i = 0;

    if (node == nullptr)
        return;
    for (auto e : node->list_next) {
        if (e->id == idnode) {
            for (auto a : e->list_next) {
                node->list_next.push_back(a);
            }
            node->list_next.removeAt(i);
            return;
        }
        i += 1;
    }
}

Q_INVOKABLE void ModuleManager::deleteNodeModule(int idnode)
{
    NodeModule *node = GetNodeModuleNodeParent(idnode);
    int i = 0;

    if (node == nullptr)
        return;
    for (auto e : node->list_next) {
        if (e->id == idnode) {
            node->list_next.removeAt(i);
            return;
        }
        i += 1;
    }
}

Q_INVOKABLE void ModuleManager::deleteNodeModuleAll()
{
    list_module->list_next.clear();
}

int ModuleManager::getSpaceRec(NodeModule *tmp)
{
    int rslt = 0;

    for (auto e : tmp->list_next) {
        if (e->list_next.size() == 0)
            rslt += 1;
        else
            rslt += getSpaceRec(e);
    }
    return rslt;
}

Q_INVOKABLE int ModuleManager::getSpace(int idnode)
{
    NodeModule *node = GetNodeModuleNode(idnode);
    int rslt = getSpaceRec(node);

    return rslt;
}

Q_INVOKABLE QList<int> ModuleManager::getNodeModule(int idnode)
{
    NodeModule *node = GetNodeModuleNode(idnode);
    QList<int> dest;

    for (auto e : node->list_next) {
        dest.push_back(e->id);
    }
    return dest;
}

Q_INVOKABLE bool ModuleManager::getNodeModuleIsNode(int idnode)
{
    NodeModule *node = GetNodeModuleNode(idnode);
    return node->node;
}

Q_INVOKABLE QList<int> ModuleManager::getNodeModule()
{
    NodeModule *node = list_module;
    QList<int> dest;

    for (auto e : node->list_next) {
        dest.push_back(e->id);
    }
    return dest;
}
