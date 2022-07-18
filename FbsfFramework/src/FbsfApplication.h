#ifndef FBSFGUIAPPLICATION_H
#define FBSFGUIAPPLICATION_H

#include "FbsfConfiguration.h"
#include "FbsfBaseModel.h"
#include "FbsfGlobal.h"
#include "FbsfPublicDataModel.h"
#ifndef MODE_BATCH
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickView>
#endif
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QString>
#include <QStringList>
#include <QDebug>

#if defined(FBSF_FRAMEWORK_LIBRARY)
#  define FBSF_FRAMEWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FBSF_FRAMEWORKSHARED_EXPORT Q_DECL_IMPORT
#endif
class FbsfSequence;
class FBSFBaseModel;
class FbsfExecutive;
class FbsfNetServer;

class FbsfGuiApplication;
class FbsfBatchApplication;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Base class for GUI, Batch or Server application
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_FRAMEWORKSHARED_EXPORT FbsfApplication: public QObject
{
    Q_OBJECT
public :
    FbsfApplication(){}
    virtual ~FbsfApplication();
    static  FbsfApplication* app(int & argc, char **argv);
    FbsfApplication(const FbsfApplication&) = delete;
    FbsfApplication& operator=(const FbsfApplication&) = delete;
    virtual void            setup()=0;

    virtual int             start(uint aPeriod=100,float aFactor=1,uint recorder=0)=0;
    virtual int             addSequence(QString aName,float aPeriod,
                                        QList<FbsfConfigNode>& aNodes,
                                        QList< QMap<QString,QString> >&,
                                        FbsfApplication* app)=0;
    virtual FbsfExecutive*  executive()=0;

    virtual QObject*        getRootWindow(){return nullptr;}

    virtual eApplicationMode    mode() =0;
    virtual void            loadQML(){}
    virtual void            setTimeDepend(bool aFlag){Q_UNUSED(aFlag)}
    virtual void            setOptPerfMeter(bool aFlag);

    static int              parseCommandLine(QStringList arglist);
    static int              parseArguments();
    static  QCommandLineParser&     parser(){return *mParser;}
    int             generateSequences();
    int             generateDataFlowGraph();

    FbsfConfiguration&              config(){return mConfig;}
    QString&                        configName() {return config().Name();}

    static QCommandLineParser       *mParser;
    static QStringList              arglist;

    static FbsfConfiguration        mConfig;
    // runtime path set from EV
    static QString                  sFrameworkHome;
    static QString                  sApplicationHome;
    static QString                  sComponentsPath;
    float   timeStep    = 0.5;
    float   speedFactor = 1.0;// n > 1  => slow time by n
    uint    recorderSize = UINT_MAX;
};
#ifndef MODE_BATCH
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Standalone application with model computation and GUI
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_FRAMEWORKSHARED_EXPORT FbsfGuiApplication
        : public QApplication
        , public QQuickView
        , public FbsfApplication
{
public:
    FbsfGuiApplication(eApplicationMode aMode,int & argc, char **argv);
    virtual ~FbsfGuiApplication()override;
    int                     start(uint aPeriod=500, float aFactor=1.0, uint aRecorder=0)override;
    int                     addSequence(QString aName,float aPeriod,
                                        QList<FbsfConfigNode>& aNodes,
                                        QList< QMap<QString,QString> >& aModels,
                                        FbsfApplication* app) override;
    virtual FbsfExecutive*  executive() override{return Executive ;}
    QObject*                getRootWindow() override {return mEngine.rootObjects().value(0);}
    eApplicationMode        mode() override {return mMode;}
    virtual void            loadQML() override;
    virtual void            setTimeDepend(bool aFlag) override;
    void                    setup() override;

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;

private :

    FbsfExecutive*          Executive;
    QQmlApplicationEngine   mEngine;
    QQuickWindow*           mWindow;
    eApplicationMode        mMode;
};
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Batch/Server application for model computation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_FRAMEWORKSHARED_EXPORT FbsfBatchApplication
        : public QCoreApplication
        , public FbsfApplication
{
public:
    FbsfBatchApplication(eApplicationMode aMode, int & argc, char **argv);
    virtual ~FbsfBatchApplication()override;
    void                    setup() override;
    int                     start(uint aPeriod=100, float aFactor=1, uint aRecorder=0)override;
    int                     addSequence(QString aName,float aPeriod,
                                        QList<FbsfConfigNode>& aNodes,
                                        QList< QMap<QString,QString> >& aModels,
                                        FbsfApplication* app)override;
    virtual FbsfExecutive*  executive()override{return Executive ;}

    eApplicationMode        mode()override {return mMode;}

private :
    FbsfExecutive*          Executive;
    eApplicationMode        mMode;
};
#endif // FBSFGUIAPPLICATION_H
