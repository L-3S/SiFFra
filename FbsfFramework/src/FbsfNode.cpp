#include "FbsfNode.h"

#include <QObject>
#define SYNCHRONE_INIT      // Trigger input on cycle start

FbsfNode::FbsfNode(): FBSFBaseModel() {
}

int FbsfNode::addSequence(QString aName, float aPeriod,
                               QList<FbsfConfigNode>& aNodes,
                               QList<QMap<QString, QString> > &aModels,
                               FbsfApplication *app)
{
    FbsfSequence* pSequence=new FbsfSequence(aName,aPeriod,app, taskCond, iterCond);

    pSequence->addModels(aModels, aNodes);
    // Create the sequence thread
    QThread* thread = new QThread;
    pSequence->moveToThread(thread);

    // connect signal to sequence slots : cycle start
    // QThread::connect();
    QThread::connect(this , SIGNAL(cycleStart()) , pSequence, SLOT(cycleStart()));
    // connect signal to sequence slots : pre/step computation and finalization
    QThread::connect(this , SIGNAL(consume()) , pSequence, SLOT(consumeData()));
    QThread::connect(this , SIGNAL(compute(int)) , pSequence, SLOT(computeStep(int)));
    // release
    QThread::connect(pSequence, SIGNAL(finished()), thread, SLOT(quit()));
    QThread::connect(thread   , SIGNAL(finished()), thread, SLOT(terminate()));
    // Message collector
    QThread::connect(pSequence, SIGNAL(error(QString)), this, SLOT(errorString(QString)));

#ifdef SYNCHRONE_INIT
    // due to QML monitoring (FbsfDataExchange must belong to this thread)
    pSequence->initialize();
#else
    // synchronize initialisation on thread started event

    connect(thread, SIGNAL(started()) , pSequence, SLOT(initialize()));
#endif
    // start thread and wait for initialisation completion

    thread->start();
#ifndef SYNCHRONE_INIT
    FbsfSequence::waitCompletion(1);// Asynchronous initialisation
#endif
    if(pSequence->status()) {
        mSequenceList.append(pSequence);// register sequence
    } else {
        thread->terminate(); // failure
    }
    return pSequence->status();
}

void FbsfNode::doCycle(int timeOut)
{
    resetWorking(mSequenceList.size());
    emit cycleStart();
    while (stillWorking() > 0) {
        // loop until all iterations completed for all sequences
        emit consume();// Synchronous consumption of inputs
        waitCompletion(mSequenceList.size());
        emit compute(timeOut);// compute models
        waitCompletion(mSequenceList.size());
    }
    emit statusChanged("compute", "stepEnd");
}

void FbsfNode::run(int timeOut)
{
    doCycle(timeOut);
}

void FbsfNode::waitCompletion(int aNbTasks)
{
    taskCond.acquire(aNbTasks);
}

void FbsfNode::resetWorking(int aNbSequences)
{
    iterCond.release(aNbSequences);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// number of working with iterations called
/// from Executive controller
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfNode::stillWorking()
{
    return iterCond.available();
}
