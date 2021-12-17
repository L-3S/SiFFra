#include "TreeModel.h"
#include "Controller.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QFile>
#include <QtWidgets/QMessageBox>


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qml);
    QCoreApplication::setOrganizationName("L3S");
    QCoreApplication::setOrganizationDomain("web.l-3s.com");
    QCoreApplication::setApplicationName("FbsfConfig");

    QApplication app(argc, argv);
    QStringList arglist;
    for (int i=0;i < argc ; i++)
    {
        QString argMode(argv[i]);
        arglist << argMode;
    }
    // Parse the command line
    Controller  controller;
    controller.parseCommandLine(arglist);

    QQmlApplicationEngine engine;
    //~~~~~~~~~~ Check application EV ~~~~~~~~~~~~~~~~~~~~~~~~~~
    QString frameworkHome = qgetenv("FBSF_HOME");
    if (frameworkHome.isEmpty())
    {
        QString msg("Environment variable FBSF_HOME not set");
        QMessageBox::critical( nullptr, "[Fatal]", msg.toStdString().c_str());
        qFatal(msg.toStdString().c_str());
    }
    engine.rootContext()->setContextProperty("FBSF_HOME",QUrl::fromLocalFile(frameworkHome));

    engine.rootContext()->setContextProperty("controller",&controller);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
