#ifndef MODULE_LOGIC_H
#define MODULE_LOGIC_H

#include "ModuleLogic_global.h"
#include "FbsfBaseModel.h"
//#include "FbsfGlobal.h"
#include <QVariant>

class MODULE_LOGIC_SHARED_EXPORT ModuleLogic
        : public FBSFBaseModel
{
public:
    ModuleLogic();
    virtual int     doInit();
    virtual int     doTerminate();
    virtual int     doStep();
    virtual int     doSaveState();                      // Save states to memory
    virtual int     doRestoreState();                   // Restore states from memory
    virtual int     doSaveState(QDataStream &out);      // dump state to file
    virtual int     doRestoreState(QDataStream& in);    // Restore state from file
    //~~~~~~~~~~~~~ parameters ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void            declareParameters();
    void            declareConstants();
    void            checkParameterChange();             // Check if public value changed
    virtual void    notifyParamChanged(QString aName, QVariant aValue);
//    virtual QList<ParamProperties> getParamList();
    virtual QMap<QString, ParamProperties> getParamList(){return mListParam;}; // TODO

private :
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Internal class for parameters and constants
    // Parameters : subscribe tunable and publish fixed
    //
    // Public data pool <-> Module copy <-> Module QVariant <-> GUI
    //
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class param
    {
    public :
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        param(QVariant val)
        {
            localValue=val;
            // force unified type for (int, float, bool)
            publishedValue=localValue.toFloat();
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        void declare(FBSFBaseModel* pModule,QString aName,QString aType)
        {
            if(aType=="tunable")
            {
                pModule->subscribeAppType(cParameter,aName,&publishedValue);
            }
            else if(aType=="fixed")
            {
                pModule->publishAppType(cConstant,aName,&publishedValue);
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // called from GUI to publish the public value
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        void setValue(FBSFBaseModel* pModule,QString aName,QVariant aValue)
        {
            localValue=aValue;
            publishedValue=localValue.toFloat();
            // push the new value to the public data pool
            pModule->publishValue(aName,publishedValue);
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Checked every step in module doStep
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        bool changed()
        {
            if(publishedValue!=localValue.toFloat())
            {
                // update copy of the local value
                if(localValue.type()==QVariant::Bool)
                    localValue.setValue(static_cast<bool>(publishedValue));
                else
                    localValue.setValue(publishedValue);
                return true;
            }
            return false;
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        QVariant value() {return localValue;}

        //~~~~~~~ members ~~~~~~~~~~~~~~~~~~~~~~~~~~
        QVariant    localValue;             // Node value
        real        publishedValue;         // Public value
    };

    QVariantMap             mParametersMap;     // Document parameters
    QVariantMap             mConstantsMap;      // Document constants
    QHash<QString,param*>   mParameters;        // Copy of public value
    QHash<QString,param*>   mConstants;         // Copy of public value

    bool                    bPublishParam;      // configuration option
    bool                    bSnapshotParam;     // configuration option

    QMap<QString, ParamProperties> mListParam =
    {{"name", ParamProperties{Param_quality::cMandatory, Param_type::cStr,
                              "Name of the module", "", "Logic1",
                              QVector<QVariant>(),"default-name"}},
     {"document",ParamProperties{Param_quality::cMandatory,
                                 Param_type::cFilePath, "Path to the qml Document",
                                 "QML files (*.qml)", "",
                                 QVector<QVariant>() ,"no-path-set"}}
    };
};
#ifdef MODULE_LOGIC_LIBRARY
extern "C" Q_DECL_EXPORT FBSFBaseModel*   factory()
{
    return new ModuleLogic();
}
#endif
#endif // MODULE_LOGIC_H
