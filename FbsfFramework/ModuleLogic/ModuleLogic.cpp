#include "ModuleLogic.h"

#include <QDebug>
#include <QString>
#include <QObject>
#include <QHash>
#include <QQuickView>
#include <QQuickItem>
#include <QDir>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModuleLogic::ModuleLogic()
    : FBSFBaseModel()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleLogic::doInit()
{
    // get the parameter options
    bPublishParam =AppConfig()["publishparam"]=="true"?true:false;
    bSnapshotParam=AppConfig()["snapshotparam"]=="true"?true:false;

    // load the Logic document in the visual engine
    int ret=0;
    ret = loadVisualDocument(QDir::currentPath()+"/"+config()["document"]);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // connect GUI signal to local slot (parameters change)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (bPublishParam)
    {
        QObject::connect(mQmlPlugin,SIGNAL(notifyParamChanged(QString,QVariant)),
                        this, SLOT(ParamChanged(QString,QVariant)));
    }
    if(ret==1) initializeVisualDocument();
    // set the parameter snapshot option if true
    if (bSnapshotParam==true)
    {
        QMetaObject::invokeMethod(mQmlPlugin, "snapParameter",
                                  Q_ARG(QVariant,bSnapshotParam));
    }
    // set the parameter publish option if true
    if (bPublishParam==true)
    {
        declareParameters();
        declareConstants();
    }
    return ret;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleLogic::doStep()
{
    // check if changes in public data pool
    if (bPublishParam)  checkParameterChange();

    computeVisualDocument();

    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleLogic::doTerminate()
{
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Save state to memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleLogic::doSaveState()
{
    return saveStateVisualDocument();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Reload state from memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleLogic::doRestoreState()
{
    return restoreStateVisualDocument();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Save state to file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleLogic::doSaveState(QDataStream &out)
{
    return saveStateVisualDocument(out);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Reload state from memory
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleLogic::doRestoreState(QDataStream &in)
{
    return restoreStateVisualDocument(in);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Declare the tunable parameters as subscribed data
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ModuleLogic::declareParameters()
{
    QVariant argParams;
    QMetaObject::invokeMethod(mQmlPlugin, "getParameters",
                              Q_RETURN_ARG(QVariant, argParams));
    mParametersMap=argParams.toMap();
    for(QVariantMap::const_iterator iter = mParametersMap.begin();
                                    iter != mParametersMap.end();
                                    ++iter)
    {
            param* p=new param(iter.value());
            mParameters.insert(iter.key(),p);
            p->declare(this,name()+"."+iter.key(),"tunable");
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Declare the fixed parameters as published data
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ModuleLogic::declareConstants()
{
    QVariant consts;
    QMetaObject::invokeMethod(mQmlPlugin, "getConstants",
                              Q_RETURN_ARG(QVariant, consts));
    mConstantsMap=consts.toMap();
    for(QVariantMap::const_iterator iter = mConstantsMap.begin();
                                    iter != mConstantsMap.end();
                                    ++iter)
    {
        param* p=new param(iter.value());
        mConstants.insert(iter.key(),p);
        p->declare(this,name()+"."+iter.key(),"fixed");
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Notified change from GUI
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ModuleLogic::notifyParamChanged(QString aName,QVariant aValue)
{
   if (bPublishParam==false) return;
   param* p=mParameters.value(aName);
   if(p!=nullptr)
   {
       // change the local and published value
       p->setValue(this,name()+"."+aName,aValue);
   }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Check any change from published parameter value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ModuleLogic::checkParameterChange()
{
    for(QHash<QString,param*>::iterator iter = mParameters.begin();
                                        iter != mParameters.end();
                                        ++iter)
    {
        if(iter.value()->changed())
        {
            QMetaObject::invokeMethod(mQmlPlugin,"setParameter",
                                      Q_ARG(QVariant, iter.key()),
                                      Q_ARG(QVariant, iter.value()->value())
                                      );
        }
    }
}
