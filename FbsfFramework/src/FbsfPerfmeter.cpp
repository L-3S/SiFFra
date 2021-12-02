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
    // Name of columns, since pre/post time have been added
    // some empty columns are inserted
    mOutStream	<< "Step;Type;Name;CpuTime"
                << "\n";
    mOutStream.flush();
    mOutFile.flush();

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPerfMeter::DumpToFile(uint stepNumber,Phase aPhase)
{
    QString perfData;
    switch (aPhase)
    {
    case cInitial:perfData=mExecutive->getPerfMeterInitial();break;
    case cStep:perfData=mExecutive->getPerfMeterStep();break;
    case cFinal:perfData=mExecutive->getPerfMeterFinal();break;
    }
    mOutStream	<< (aPhase==cInitial?"Init":
                                     aPhase==cFinal?"Final":
                                                    "step "+QString::number(stepNumber))
                << ";Executive;"
                << "" << ";"
                << perfData
                << "\n";
    mOutStream.flush();
    DumpSequenceTree(stepNumber,aPhase,mExecutive->sequences());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPerfMeter::DumpSequenceTree(uint stepNumber,Phase aPhase,
                                     QList<FbsfSequence*> aSeqList)
{
    for(int s=0;s<aSeqList.size();s++)
    {
        QString perfData;
        FbsfSequence* seq=aSeqList[s];
        switch (aPhase)
        {
        case cInitial:perfData=seq->getPerfMeterInitial();break;
        case cStep:perfData=seq->getPerfMeterStep();break;
        case cFinal:perfData=seq->getPerfMeterFinal();break;
        }
        mOutStream	<< (aPhase==cInitial?"Init":
                                         aPhase==cFinal?"Final":
                                                        "step "+QString::number(stepNumber))
                    << ";sequence;"
                    << seq->name() << ";"
                    << perfData
                    << "\n";
        mOutStream.flush();

        for(int m=0;m<seq->getModelList().size();m++)
        {

            FBSFBaseModel* mod=seq->getModelList()[m];

            if (mod->isNode())
            {
                FbsfNode* node=dynamic_cast<FbsfNode*>(mod);// down cast
                DumpSequenceTree(stepNumber,aPhase,node->sequences());
                continue;// do not record node time
            }

            switch (aPhase)
            {
            case cInitial:perfData=mod->getPerfMeterInitial();break;
            case cStep:perfData=mod->getPerfMeterStep();break;
            case cFinal:perfData=mod->getPerfMeterFinal();break;
            }
            mOutStream	<< ";module;"
                        << mod->name() << ";"
                        << perfData
                        << "\n";
            mOutStream.flush();
        }
    }
    mOutFile.flush();
}

