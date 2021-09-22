#ifndef NODE_H
#define NODE_H


#include <QThread>
#include <QString>
#include <QDebug>

#include "ModuleLogic/ModuleLogic_global.h"
#include "FbsfBaseModel.h"
#include "FbsfGlobal.h"
#include "FbsfSequence.h"
#include "FbsfApplication.h"
class FbsfNode
        : public FBSFBaseModel
{
    Q_OBJECT
public:
    FbsfNode();
    virtual int     doInit() {return 1;};
    virtual int     doTerminate() {return 1;};
    virtual int     doStep() {
        run();
        return 1;
        };
    virtual int     doSaveState() {return 1;};                      // Save states to memory
    virtual int     doRestoreState() {return 1;};                   // Restore states from memory
    virtual int     doSaveState(QDataStream &out) {return 1;};      // dump state to file
    virtual int     doRestoreState(QDataStream& in) {return 1;};    // Restore state from file


    int             addSequence(QString aName, float aPeriod,
                               QList<FbsfConfigNode>& aNodes,
                               QList<QMap<QString, QString> > &aModels,
                               FbsfApplication *app);
    void            doCycle();                    // execute one cycle

    // for perfMeter
    QList<FbsfSequence*>& sequences(){return mSequenceList;}
    virtual bool    isNode() override {return true;}

public slots:
    void            run();                      // run the executive workflow (thread)
    // void            control(QString command, QString param1=QString(),QString param2=QString());   // control of the simulation workflow
    void            errorString(QString err) {
        qDebug() << "[Error] : " <<  err;
    };   // error message collector

signals:
    void            cycleStart();               // signal start of major cycle
    void            consume();                  // signal data consumption
    void            compute();                  // signal step computation
    void            statusChanged(QVariant  mode,QVariant  state);// signal status change to UI
    void            exit();

private:
    void     waitCompletion(int aNbTasks);
    void resetWorking(int aNbSequences);
    int stillWorking();

    QList<FbsfSequence*>    mSequenceList;       // Sequence list
    QSemaphore   taskCond;
    QSemaphore   iterCond;

};
#endif //NODE_H
