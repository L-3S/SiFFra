#include "TypeModule.h"

TypeModule::TypeModule()
{

}


//void TypeModule::addParams(QString name, QString type)
//{
//    vector2s tmp;
//    tmp.type = type;
//    tmp.name = name;
//    _params.push_back(tmp);
//}
void TypeModule::addParams(QMap<QString, ParamProperties> listParam)
{
    QMap<QString,ParamProperties>::const_iterator iter = listParam.constBegin();
    while (iter != listParam.constEnd()) {
        _valueparams[iter.key()] = "";
        ++iter;
    }
    _params = listParam;
}


void TypeModule::addParamsValue(QString name, QString value)
{
//    vector2s tmp;
//    tmp.value = value;
//    tmp.name = name;
//    _params.push_back(tmp);
    _valueparams[name] = value;
}


void TypeModule::addName(QString name)
{
    _name = name;
}


QString TypeModule::getName()
{
    return _name;
}

void TypeModule::dump()
{
    std::cout << qPrintable(_name) << std::endl;
    QMap<QString,ParamProperties>::const_iterator iter = _params.constBegin();
    while (iter != _params.constEnd()) {
        std::cout << qPrintable(iter.key() + " " + iter->mHint.toString()) << std::endl;
        ++iter;
    }




//    for (auto e : _params) {
//        std::cout << qPrintable(e.name + " " + e.type) << std::endl;
//    }
    std::cout << std::endl;
}

QString TypeModule::getType()
{
    return _name;
//    for (auto e : _params) {
//        if (e.name == "type")
//            return e.value;
////        std::cout << qPrintable(e.name + " " + e.type) << std::endl;
//    }
    return "undifined";
}

//void TypeModule::loadTypeFromBase(QString name)
//{
//    TypeModule* newType;
//    newType = _managetype->getType(name);
//    if (newType != nullptr) {
//        _name = newType->getName();
//        _params = newType->_params;
//    }
//}

void TypeModule::checkLib()
{
    QMap<QString, QVariant>::const_iterator iter = _valueparams.constBegin();
    FBSFBaseModel* tmp;
    TypeModule* newType;

    std::cout << _valueparams.size() << std::endl;

    while (iter != _valueparams.constEnd()) {
        if (iter.key().toLower() == "module") {
            newType = _managetype->getType(iter.value().toString());
            if (newType != nullptr) {
                _name = newType->getName();
                _params = newType->_params;
                return;
            } else {
                for (auto e : _managetype->_pathtodll)
                    tmp = FBSFBaseModel::loadModule(e + iter.value().toString(), iter.value().toString(), 0); // TODO SELECT OTHER PATH FOR LOAD LIBS
                if (tmp != nullptr) {
                    _managetype->CreateType(tmp->getParamList(), iter.value().toString());
                    _name = iter.value().toString();
                    _params = tmp->getParamList();
                    return;
                } else {
                    _name = "not found";
                    std::cout << "module not found add the dll " << "lib" + iter.value().toString().toStdString() << std::endl;
                }
            }
        } else {
            _name = "not found";
        }
        ++iter;
    }
}

