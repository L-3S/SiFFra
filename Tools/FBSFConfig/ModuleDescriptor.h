#ifndef MODULEDESCRIPTOR_H
#define MODULEDESCRIPTOR_H

#include "ParamProperties.h" // include from FBSF
#include "ItemProperties.h"

#include <QObject>
#include <QMap>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ModuleDescriptor : public QObject
{
    Q_OBJECT
public :
    ModuleDescriptor(QString aCategory,
                     QString aTypeCpp,
                     QString aFilePath);

    Q_PROPERTY(QString category READ category CONSTANT)
    Q_PROPERTY(QString typeCpp READ typeCpp CONSTANT)

    // Accessors
    QString category() {return mCategory;}
    QString typeCpp() {return mTypeCpp;}
    QString filePath() {return mFilePath;}

    // Module properties
    static const QMap<QString, ParamProperties> &getModuleTypeProperties(QString aType);

private :
    QString mCategory;
    QString mTypeCpp;
    QString mFilePath;
    QMap<QString, ParamProperties>  mProperties;

    static QMap<QString,ModuleDescriptor*> sModuleTypeMap;

};


#endif // MODULEDESCRIPTOR_H
