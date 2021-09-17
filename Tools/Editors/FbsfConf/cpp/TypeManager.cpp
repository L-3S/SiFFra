#include "TypeManager.h"


void TypeManager::addType(TypeModule newType)
{
    listType.push_back(newType);
}

//void TypeManager::addType(QMap<QString, ParamProperties> listParam)
//{
//    listType.push_back(listParam);
//}



void TypeManager::dump()
{
    for (auto e : listType)
    {
        e.dump();
    }
}


Q_INVOKABLE QString TypeManager::getByIndex(int index)
{
    std::cout << qPrintable(listType[index].getName()) << std::endl;
    return listType[index].getName();
}


Q_INVOKABLE int TypeManager::getSize()
{
    return listType.size();
}


TypeModule TypeManager::getTypeByIndex(int index)
{
    return listType[index];
}


void TypeManager::CreateType(QMap<QString, ParamProperties> listParam, QString name)
{
    TypeModule tmp;

    tmp.addName(name); // TODO
    tmp.addParams(listParam);
    addType(tmp);
}

TypeModule *TypeManager::getType(QString nameType)
{
    for (auto &e : listType)
    {
        if (e.getName() == nameType)
            return &e;
    }
    return nullptr;
}


void TypeManager::loadLib(QString path)
{
    QString name = path.split("lib")[path.split("lib").size() -1];
    name = name.split(".")[0];
    std::cout << name.toStdString() << std::endl;
    FBSFBaseModel* newmodule = FBSFBaseModel::loadModule(path, name, 0);
    if (newmodule == nullptr)
        return;
    CreateType(newmodule->getParamList(), name);
}
