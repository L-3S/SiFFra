#ifndef NODEMODULE_H
#define NODEMODULE_H

#include <QObject>
#include "TypeModule.h"

class NodeModule {

public:
    explicit NodeModule  () {
    };
    int id = 0;
    bool node = false;
    bool endNode = false;
    bool sequence = false;
    QString name = "a";
    TypeModule infosType;
    QList<NodeModule*> list_next;
};

#endif // NODEMODULE_H
