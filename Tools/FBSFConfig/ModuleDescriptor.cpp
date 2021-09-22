#include "ModuleDescriptor.h"
#include "FbsfBaseModel.h"
#include <QDebug>
#include <QLibrary>

#define GET_FROM_LIB

QMap<QString,ModuleDescriptor*> ModuleDescriptor::sModuleTypeMap;
static QMap<QString, ParamProperties> emptyProperties;

ModuleDescriptor::ModuleDescriptor(QString aCategory,
                                   QString aTypeCpp,
                                   QString aFilePath)
    : mCategory(aCategory), mTypeCpp(aTypeCpp),
      mFilePath(aFilePath)
{
    sModuleTypeMap[aTypeCpp]=this;// static for TreeItem access

    // internally defined (see ItemParams.h)
    if(aFilePath.isEmpty()){
        mProperties=paramModuleMap.value(aTypeCpp);
        qDebug() << aTypeCpp << aCategory
                 << "properties defined internally";
        return;
    }
    // get the properties from the module library
    QLibrary sharedLibrary(aTypeCpp);
    qDebug() << aTypeCpp << aCategory
             << "properties defined from"
             << aFilePath;

    typedef  FBSFBaseModel* (*ModelFactory)();
    QFunctionPointer fp=sharedLibrary.resolve("factory");

    if(fp==nullptr)
    {
        qDebug()<<__FUNCTION__ <<  "Error while loading module" << aTypeCpp;
        qDebug() << sharedLibrary.errorString();
        return;
    }

    ModelFactory factoryFunction =
            reinterpret_cast<ModelFactory>( fp );

    if (factoryFunction)
    {
        FBSFBaseModel* pModel = factoryFunction();
        mProperties=pModel->getParamList();
    }
    else
    {
        qDebug()<<__FUNCTION__ <<  "Error while loading module" << aTypeCpp;
        qDebug() << sharedLibrary.errorString();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const QMap<QString, ParamProperties>& ModuleDescriptor::getModuleTypeProperties(QString aType)
{
    if(sModuleTypeMap.contains(aType))
    {
        return sModuleTypeMap.value(aType)->mProperties;
    }
    else
    {
        qDebug() << __FUNCTION__ << "no properties found for type " << aType;
        return emptyProperties;
    }
}

