#ifndef FBSFPERFMETER_H
#define FBSFPERFMETER_H

#include <QString>
#include <QTextStream>
#include <QFile>

class FbsfExecutive;
class FbsfSequence;
class FbsfPerfMeter
{
public:

    enum Phase{
        cHeader,
        cInitial,
        cStep,
        cFinal
    };
    FbsfPerfMeter(FbsfExecutive* aExecutive);
    ~FbsfPerfMeter();

    void openFile(QString aFileName);

    void DumpToFile(uint stepNumber, Phase aPhase);
    void DumpSequenceTree(QString& perfData, uint stepNumber, Phase aPhase,
                          QList<FbsfSequence *> &aSeqList);

private:

    FbsfExecutive*  mExecutive;
    QFile           mOutFile;
    QTextStream     mOutStream;
    bool            mActive=false;
};

#endif // FBSFPERFMETER_H
