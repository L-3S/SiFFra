#ifndef TESTAPI_H
#define TESTAPI_H

#include <iostream>
#include "FbsfApi.h"

using namespace std;
class TestAPI: public QObject{
    Q_OBJECT
public:
    explicit TestAPI(QObject* parent = 0) : QObject(parent) {}
    void *comp;
    FbsfApi api;
};

#endif // TESTAPI_H
