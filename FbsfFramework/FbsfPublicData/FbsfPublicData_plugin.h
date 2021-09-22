#ifndef FBSF_PUBLIC_DATA_PLUGIN_H
    #define FBSF_PUBLIC_DATA_PLUGIN_H

#include <QtCore/qglobal.h>     // QT_VERSION, Q_DECL_EXPORT, Q_DECL_IMPORT
#include <QObject>
#ifndef MODE_BATCH
#include <QQmlExtensionPlugin>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfPublicDataPlugin
	: public QQmlExtensionPlugin
{
	Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
#endif

public:
	void	registerTypes( const char *uri );
} ;
#endif
#endif // FBSF_PUBLIC_DATA_PLUGIN_H
