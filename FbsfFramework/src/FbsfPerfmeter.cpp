#include "FbsfPerfmeter.h"
#include "FbsfExecutive.h"
#include "FbsfSequence.h"
#include "FbsfNode.h"
FbsfPerfMeter::FbsfPerfMeter(FbsfExecutive* aExecutive)
    : mExecutive(aExecutive)
{

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfPerfMeter::~FbsfPerfMeter()
{
    mOutFile.close();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPerfMeter::openFile(QString aFileName)
{
    mOutFile.setFileName(aFileName);

    if (!mOutFile.open(QIODevice::ReadWrite|QIODevice::Truncate))
        qDebug() << __FUNCTION__ << mOutFile.errorString() ;
    mOutStream.setDevice(&mOutFile);
    mActive=true;
    // Name of columns
    DumpToFile(0,cHeader);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPerfMeter::DumpToFile(uint stepNumber,Phase aPhase)
{
    QString perfData;
    switch (aPhase)
    {
    case cHeader :  perfData = "Phase;Executive";break;
    case cInitial:  perfData="Init;"+mExecutive->getPerfMeterInitial();break;
    case cStep:     perfData="step "+QString::number(stepNumber)+";"
                            +mExecutive->getPerfMeterStep();break;
    case cFinal:    perfData="Final;"+mExecutive->getPerfMeterFinal();break;
    }

    DumpSequenceTree(perfData,stepNumber,aPhase,mExecutive->sequences());
    mOutStream << perfData << "\n";
    mOutStream.flush();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPerfMeter::DumpSequenceTree(QString& perfData,uint stepNumber,
                                     Phase aPhase,
                                     QList<FbsfSequence*>& aSeqList)
{
    for(int s=0;s<aSeqList.size();s++)
    {
        FbsfSequence* seq=aSeqList[s];
        switch (aPhase)
        {
        case cHeader :  perfData+=";["+seq->name()+"]";break;
        case cInitial:  perfData+=";"+seq->getPerfMeterInitial();break;
        case cStep:     perfData+=";"+seq->getPerfMeterStep();break;
        case cFinal:    perfData+=";"+seq->getPerfMeterFinal();break;
        }

        for(int m=0;m<seq->getModelList().size();m++)
        {

            FBSFBaseModel* mod=seq->getModelList()[m];

            if (mod->isNode())
            {
                FbsfNode* node=dynamic_cast<FbsfNode*>(mod);// down cast
                DumpSequenceTree(perfData,stepNumber,
                                 aPhase,node->sequences());
                continue;// do not record node time
            }

            switch (aPhase)
            {
            case cHeader :  perfData+=";"+mod->name() ;break;
            case cInitial:  perfData+=";"+mod->getPerfMeterInitial();break;
            case cStep:     perfData+=";"+mod->getPerfMeterStep();break;
            case cFinal:    perfData+=";"+mod->getPerfMeterFinal();break;
            }
        }
    }
}


