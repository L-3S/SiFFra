#include "FbsfApplication.h"
#include "FbsfExecutive.h"
#include "FbsfSequence.h"
#include "FbsfPublicData.h"
#ifndef MODE_BATCH
#include "FbsfPublicDataModel.h"//sPublicDataModel
#include <QQmlContext>
#include <QtWidgets/QMessageBox>
#endif
#include <iostream>
#include <QCoreApplication>
#include <QDir>

QCommandLineParser      *FbsfApplication::mParser;
QStringList             FbsfApplication::arglist;
QString                 FbsfApplication::sFrameworkHome;
QString                 FbsfApplication::sApplicationHome;
QString                 FbsfApplication::sComponentsPath;
FbsfConfiguration       FbsfApplication::mConfig;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \class FbsfApplication
/// \brief Base class for FBSF application
///        operating mode : standalone, client or server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfApplication::~FbsfApplication()
{
    arglist.clear();
    mConfig = FbsfConfiguration();
    sFrameworkHome = "";
    sApplicationHome = "";
    sComponentsPath = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfApplication *FbsfApplication::app(int & argc, char **argv)
{
    mParser = new QCommandLineParser();
    eApplicationMode mode=standalone;
    for (int i=0;i < argc ; i++)
    {
        QString argMode(argv[i]);
        arglist << argMode;
    }
    // Parse the command line
    FbsfApplication::parseCommandLine(arglist);

    if (parser().isSet(QCommandLineOption("server")))
    {mode = server;}
    else {
        if (parser().isSet(QCommandLineOption("client")))
       {mode = client;}
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check application EV
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sFrameworkHome = qgetenv("FBSF_HOME");
    if (sFrameworkHome.isEmpty())
    {
        QString msg("Environment variable FBSF_HOME not set");
#ifndef MODE_BATCH
        qFatal(msg.toStdString().c_str());
//        QMessageBox::critical( nullptr, "[Fatal]", msg.toStdString().c_str());
#endif
        qFatal(msg.toStdString().c_str());
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sApplicationHome = qgetenv("APP_HOME");
    if (sApplicationHome.isEmpty())
    {
        QString msg("Environment variable APP_HOME not set");
        qFatal(msg.toStdString().c_str());
    }
#ifndef MODE_BATCH
    switch (mode)
    {
    case standalone:
    case client:
        return new FbsfGuiApplication(mode,argc,argv);
        break;
    case server :
        return new FbsfBatchApplication(mode,argc,argv);
        break;
    default : return nullptr;
    }
#else
    return new FbsfBatchApplication(batch,argc,argv);
#endif
}

int FbsfApplication::parseArguments() {
    const QStringList args = FbsfApplication::parser().positionalArguments();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check if the configuration file is available
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (args.isEmpty())
    {
        QString msg="Argument 'configuration file.xml' is missing.\n";
//                  + FbsfApplication::parser().helpText();
        qCritical(msg.toStdString().c_str());
        return 1;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // capture sdtout and stderr from C libraries
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (!FbsfApplication::parser().isSet(QCommandLineOption("no-logfile")))
    {
        QFileInfo fi(args[0]);
        QString Clogfile = fi.baseName()+".log"; //TODO PATH CONFIGURATION
        freopen (Clogfile.toStdString().c_str(),"w",stdout);
        freopen (Clogfile.toStdString().c_str(),"a",stderr);
    }
    else
    {
        qInstallMessageHandler(0);// restore default handler
    }
    mConfig.Name() = QDir::currentPath() + "/" + args[0];
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Commande line parsing
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfApplication::parseCommandLine(QStringList arglist)
{
    QCoreApplication::setOrganizationName("L3S");
    QCoreApplication::setOrganizationDomain("web.l-3s.com");
    QCoreApplication::setApplicationName("FbsfFramework");
    QCoreApplication::setApplicationVersion(FBSF_VERSION);

    mParser->setApplicationDescription(QCoreApplication::translate("main","Fmu based simulation framework"));
    mParser->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    mParser->addHelpOption();
    mParser->addVersionOption();
    mParser->addPositionalArgument("Configuration file",
                                  QCoreApplication::translate("main", " : xml description file"));

    QCommandLineOption noLogfileOption(QStringList() << "n" << "no-logfile",
                                       QCoreApplication::translate("main", "Don't log to file."));
    mParser->addOption(noLogfileOption);// no logfile

    QCommandLineOption replayModeOption(QStringList() << "r" << "replay",
                                        QCoreApplication::translate("main", "replay simulation from <file>"),
                                        QCoreApplication::translate("main", "file"));
    mParser->addOption(replayModeOption);// replay

    QCommandLineOption noGuiOption(QStringList() << "ng" << "no-gui",
                                       QCoreApplication::translate("main", "running batch mode"));
    mParser->addOption(noGuiOption);// noGui

    QCommandLineOption clientOption(QStringList() << "c" << "client",
                                    QCoreApplication::translate("main", "Run as network client"));
    mParser->addOption(clientOption); // client mode

    QCommandLineOption serverOption(QStringList() << "s" << "server",
                                    QCoreApplication::translate("main", "Run as network server"));
    mParser->addOption(serverOption); // server

    mParser->parse(arglist);
    return 1;
}
int FbsfApplication::generateSequences() {
    setup();
    // check modeMcp

    bool modeMcp=config().Simulation().value("simuMpc")=="true"?true:false;
    setTimeDepend(modeMcp);

    // check option perfMeter
    bool optPerfMeter=config().Simulation().value("perfMeter")=="true"?true:false;
    setOptPerfMeter(optPerfMeter);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // creation of the QML viewer UI
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    loadQML();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get the simulation parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qInfo() << "Framework version : " << FBSF_VERSION;
    qInfo() << "Current working path:" << QDir::currentPath();
    qInfo() << "Configuration file :" << configName();

    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qInfo() << "Application path :" << sApplicationHome;
    qInfo() << "Components path :" <<  sComponentsPath;
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qInfo() << "Simulation parameters :";
    foreach ( const QString& key, config().Simulation().keys())
    {
        QString value=config().Simulation().value(key);
        qInfo() << "\t" << key << value;
        if (key == "timestep")      timeStep=value.toFloat();
        if (key == "speedfactor")   speedFactor=value.toFloat();
        if (key == "recorder")      recorderSize=value.toUInt();
    }
    qInfo() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // network client : should it be restricted to UI only ???
    if (mode()==client) return start(timeStep*1000,1,recorderSize);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get the sequences description
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QList<FbsfConfigSequence>::const_iterator iSeq;
    for (iSeq = config().Sequences().begin(); iSeq != config().Sequences().end(); ++iSeq)
    {
        FbsfConfigSequence sequence=*iSeq;
        QString sequenceName;

        float sequencePeriod=1;//default period to 1
        QMap<QString,QString> vMap = sequence.Descriptor();
        foreach ( const QString& key, vMap.keys())
        {
            QString value=vMap.value(key);

            if (key == "name")      sequenceName=value;
            if (key == "period")    sequencePeriod=value.toFloat();
        }
        addSequence(sequenceName,
                         sequencePeriod,
                         sequence.Nodes(),
                         sequence.Models(),
                         this);

    } // end Sequences

    // check option dataFlowGraph aand genrate the graph in a file
    bool optDataFlowGraph=config().Simulation().value("dataFlowGraph")=="true"?true:false;
    if (optDataFlowGraph)
        generateDataFlowGraph();

    return 1;
}

// DataFlowGraph
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfApplication::generateDataFlowGraph()
{
    QFile outFile;
    QString nameFile = QDir::currentPath() + "/DataFlowGraph.dot";
    outFile.setFileName(nameFile);
    if (!outFile.open(QIODevice::ReadWrite|QIODevice::Truncate))
        qDebug() << __FUNCTION__ << outFile.errorString() ;
    QTextStream outStream;
    outStream.setDevice(&outFile);

    outStream << "digraph fbsf_graphe {";
    foreach (FbsfDataExchange* data, FbsfDataExchange::sPublicDataMap)
    {
        // dump exported public data
        if (data->FlagsAny(FbsfDataExchange::cExporter))
        {
            if(!data->m_consumers.isEmpty())// only if consumers
            {
                QList<QString> consumersList = data->m_consumers.split(","); // one producer for n consumers
                for (int i = 0; i < consumersList.size(); i++)
                {
                    outStream << "\n\t" << data->producer() << " -> " << consumersList[i]
                    << "\t [Name =\"" << data->name() <<"\"]"
                    << "\t [Type = "  << config().getDataFlowEdgeStatus(data->producer(),consumersList[i]) << "]";
                }
            }
        }
    }
    outStream << "\n}";
    outStream.flush();
    outFile.flush();
    outFile.close();

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfApplication::setOptPerfMeter(bool aFlag)
{
    executive()->setOptPerfMeter(aFlag);
}

#ifndef MODE_BATCH
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \class FbsfGuiApplication
/// \brief Standalone application with model computation and GUI
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfGuiApplication::FbsfGuiApplication(eApplicationMode aMode,int & argc,char** argv)
    : QApplication(argc, argv)
    , QQuickView(nullptr)
    , mEngine()
    , mWindow(nullptr)
    , mMode(aMode)
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Executive = new FbsfExecutive(aMode);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef BUILD_API
    setup();
#endif

}
void FbsfGuiApplication::setup() {
#ifndef MODE_BATCH
    QQmlContext *ctxt = mEngine.rootContext();
    // Path acces in QML
    ctxt->setContextProperty("FBSF_HOME",QUrl::fromLocalFile(sFrameworkHome));
    ctxt->setContextProperty("APP_HOME",QUrl::fromLocalFile(sApplicationHome));
    ctxt->setContextProperty("FBSF_CONFIG",QUrl::fromLocalFile(mConfig.Name() + ".xml"));
    ctxt->setContextProperty("CURRENT_DIR",QUrl::fromLocalFile(QDir::currentPath()));
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // QML data exchange listmodel binding
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ctxt->setContextProperty("FbsfDataModel", FbsfdataModel::sFactoryListviewDataModel());
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // add import paths for plugins
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    mEngine.addImportPath(qApp->applicationDirPath());
    mEngine.addImportPath("qrc:/GraphicEditor");
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // add import paths for components
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sComponentsPath=qgetenv("COMPONENTS_PATH");

    if (sComponentsPath.isEmpty()) sComponentsPath = sFrameworkHome+"/Library";

    if (!QDir(sComponentsPath).exists())
    {
        QString msg("Invalid component path :\n"+sComponentsPath);
        QMessageBox::warning( nullptr, "[Fatal]", msg.toStdString().c_str());
        qWarning(msg.toStdString().c_str());
    }
    mEngine.addImportPath(sComponentsPath);
    ctxt->setContextProperty("COMPONENTS_PATH",QUrl::fromLocalFile(sComponentsPath));
    //~~~~~~~~~~~~~~~~~~~~~~~~~ setup UI ~~~~~~~~~~~~~~~~~~~~~~~~~
    setResizeMode(QQuickView::SizeRootObjectToView);
#endif
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfGuiApplication::~FbsfGuiApplication()
{
    delete Executive;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfGuiApplication::loadQML()
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // creation of the QML viewer UI
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    mEngine.load(QUrl("qrc:/qml/FbsfMain.qml"));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfGuiApplication::setTimeDepend(bool aFlag)
{
    QQmlContext *ctxt = mEngine.rootContext();
    FbsfDataExchange::simuMpc = aFlag;
    // Path acces in QML
    ctxt->setContextProperty("simuMpc",aFlag);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Sequence registration
/// Initialisation step for all modules
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfGuiApplication::addSequence(QString aName, float aPeriod,
                                    QList<FbsfConfigNode>& aNodes,

                                    QList<QMap<QString, QString> > &aModels,
                                    FbsfApplication* aApp)
{
    return Executive->addSequence(aName,aPeriod, aNodes, aModels,aApp);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Run application loop with a defined period (ms)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfGuiApplication::start(uint aPeriod, float aFactor, uint aRecorder)
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~ check for a replay option file ~~~~~~~
    // This code must be executed before loading the main QML file
    // and after models declarations
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (FbsfApplication::parser().isSet(QCommandLineOption("replay")))
    {
        const QString replayFile =
                FbsfApplication::parser().value(QCommandLineOption("replay"));
        Executive->ReplayMode(true,replayFile);
    }
    // Get the top level UI object
    QObject *topLevel = mEngine.rootObjects().first();
    mWindow = qobject_cast<QQuickWindow *>(topLevel);
    if (mWindow==nullptr) qFatal("Unable to load the main qml file");

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set the executive computation parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Executive->Period(aPeriod);
    Executive->fastSpeed(aFactor);
    Executive->Recorder(aRecorder);

    if (FbsfApplication::parser().isSet(QCommandLineOption("no-gui")))
    {
        // running batch mode
        Executive->BatchMode(true);
        mWindow->hide();
    }
    else
    {
        mWindow->show();
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // connect local gui control to executive workflow
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QObject::connect(topLevel,SIGNAL(guiControl(QString,QString,QString)),
                     Executive, SLOT(control(QString,QString,QString)));
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // connect executive thread exit to close the application
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QObject::connect(Executive, SIGNAL(exit()),
                     static_cast<QGuiApplication*>(this), SLOT(quit()));
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // connect executive workflow to local gui status
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QObject::connect(Executive, SIGNAL(statusChanged(QVariant ,QVariant )),
                     topLevel,SLOT(statusChanged(QVariant ,QVariant )));

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // start the executive computation workflow
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Executive->start();
    qInfo() << "Starting GUI application with timestep : "<< aPeriod << "ms";
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // start the graphic interface
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int ret = QGuiApplication::exec();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // leave the process
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if( Executive->BatchMode())
        qInfo() << "Exiting batch mode with status : "<< ret;
    else
        qInfo() << "Exiting GUI application with status : "<< ret;

    if(!Executive->ReplayMode()&& Executive->Recorder()>0)
    {
        // write the replay file in the current dir
        QString vFile;
        vFile = "file:///" + QDir::currentPath() + "/"+ configName() +".dat";
        FbsfDataExchange::writeDataToFile(vFile);
        qInfo() << "Dumped replay file : "<< vFile;
    }

    return ret;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// catch a close window event
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfGuiApplication::event(QEvent *e)
{
    int type = e->type();
    if (type == QEvent::Close || type == 20)
        Executive->control("stop","");

    return QWindow::event(e);
}
#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// \class FbsfServerApplication
/// \brief Base class Server application for model computation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfBatchApplication::FbsfBatchApplication(eApplicationMode aMode,int & argc,char** argv)
    : QCoreApplication(argc, argv)
    , mMode(aMode)
{
    Executive = new FbsfExecutive(aMode);
    // running batch mode
    if(aMode==batch) Executive->BatchMode(true);

    QObject::connect(Executive, SIGNAL(exit()),
                     dynamic_cast<QCoreApplication*>(this), SLOT(quit()));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfBatchApplication::~FbsfBatchApplication()
{
    delete Executive;
}
void FbsfBatchApplication::setup() {

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Sequence registration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfBatchApplication::addSequence(QString aName, float aPeriod,
                                       QList<FbsfConfigNode>& aNodes,
                                       QList<QMap<QString, QString> > & aModels,
                                       FbsfApplication* app)
{
    return Executive->addSequence(aName,aPeriod, aNodes, aModels,app);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Run Server/Batch application loop with a defined period
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfBatchApplication::start(uint aPeriod, float aFactor,uint aRecorder)
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // start the executive computation workflow
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Executive->Period(aPeriod);
    Executive->fastSpeed(aFactor);
    Executive->Recorder(aRecorder);
    Executive->start();

    if(mMode==batch)
        qInfo() << "Starting batch application with time step : "<< aPeriod;
    else
        qInfo() << "Starting server application with time step : "<< aPeriod;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // start the server executive
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int ret = QCoreApplication::exec();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // leave the process
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(mMode==batch)
        qInfo() << "Exiting batch application with status : "<< ret;
    else
        qInfo() << "Exiting server application with status : "<< ret;

    return ret;
}
