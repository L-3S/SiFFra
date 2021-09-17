#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>
#include <QPushButton>
#include <QQuickWindow>
#include <QQuickStyle>
#include "ModuleManager.h"
#include "XmlApi.h"
#include "LoaderType.h"

#include <iostream>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    qmlRegisterType<MyObject>("com.myself", 1, 0, "MyObject");
    QGuiApplication app (argc, argv);

    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;


    TypeManager managetype;
    LoaderType::loadLibShared(&managetype);
    engine.rootContext()->setContextProperty("managetype", &managetype);

    XmlApi xmlapi;

    ModuleManager moduleManager;
    xmlapi.spaceManager = &moduleManager;
    moduleManager.managetype = &managetype;
    engine.rootContext()->setContextProperty("spaceManager", &moduleManager);

    engine.rootContext()->setContextProperty("xmlapi", &xmlapi);

    if (argc == 2)
        xmlapi.pathloaded = QString(argv[1]);
    engine.load(QUrl(QStringLiteral("qrc:/qml/MainQml.qml")));

    QObject *topLevel = engine.rootObjects().first();
    QQuickWindow * window   = qobject_cast<QQuickWindow *>(topLevel);


    if (window) window->show();
        else qFatal("Unable to load the main qml file");


    return app.exec();
}
