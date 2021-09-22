#ifndef FBSF_UTILITIES_PLUGIN_H
#define FBSF_UTILITIES_PLUGIN_H

#include <QQmlExtensionPlugin>

class FbsfUtilitiesPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // FBSF_UTILITIES_PLUGIN_H

