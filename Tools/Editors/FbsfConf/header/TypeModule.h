#ifndef TYPEMODULE_H
#define TYPEMODULE_H

#include "QObject"
#include "Vector2S.h"
#include "iostream"
#include <vector>
#include "ParamProperties.h"
#include <QMap>
#include "FbsfBaseModel.h"
#include "TypeManager.h"

class TypeManager;
class TypeModule
{
public:
    TypeModule();

    void addParams(QMap<QString, ParamProperties> listParam);
    void addManageType(TypeManager *managetype) {_managetype = managetype;};
    void addName(QString name);
    QString getName();
    void addParamsValue(QString name, QString value);
    QString getType();
    void checkLib();

    void dump();


    //    std::vector<ParamProperties> _params;
    QMap<QString, ParamProperties> _params;
    QMap<QString, QVariant> _valueparams;
    TypeManager *_managetype = nullptr;
private:
    QString _name;

//    void loadTypeFromBase(QString);
};

#endif // TYPEMODULE_H
