#ifndef TYPEMANAGER_H
#define TYPEMANAGER_H

#include "TypeModule.h"
#include <vector>
class TypeModule;

class TypeManager : public QObject
{
    Q_OBJECT
public:
    explicit TypeManager (QObject* parent = 0) : QObject(parent) {
        _pathtodll.push_back("../lib/lib");
    };

    void addType(TypeModule newType);
    void dump();

    void CreateType(QMap<QString, ParamProperties> listParam, QString name);
    TypeModule *getType(QString nameType);

    Q_INVOKABLE void loadLib(QString path);



    Q_INVOKABLE QString getByIndex(int index);
    Q_INVOKABLE int getSize();
    TypeModule getTypeByIndex(int index);



    QList<QString> _pathtodll;
private:
    std::vector<TypeModule> listType;


};

#endif // TYPEMANAGER_H
