#ifndef LOADERTYPE_H
#define LOADERTYPE_H

#include "QObject"
#include <QDir>
#include <iostream>
#include <QMap>
#include "TypeModule.h"
#include "TypeManager.h"
#include "FbsfBaseModel.h"

class LoaderType
{
public:
    LoaderType();

    static void loadLibShared(TypeManager *managetype);
};

#endif // LOADERTYPE_H
