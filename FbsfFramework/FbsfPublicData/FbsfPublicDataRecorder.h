#ifndef FBSFPUBLICDATARECORDER_H
#define FBSFPUBLICDATARECORDER_H

#include <QList>
#include <QVariantList>
#include <QFile>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// This class is used for public data recording (historical)
//
//
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FbsfDataExchange;
class FbsfPublicDataRecorder
{
public:
    FbsfPublicDataRecorder(FbsfDataExchange*aData, int aTimeIndex, int aTimeShift);
    ~FbsfPublicDataRecorder();
    FbsfPublicDataRecorder(const FbsfPublicDataRecorder &obj) ;


    void                record(int aStep);
    void                setBacktrack(int aStep);
    void                serialize(QDataStream& aStream);
    void                deserialize(QDataStream& aStream);
    int                 historySize();
    QVariantList &      history();
    QVariantList &      historywithfutur();
    QVariantList &      history(int aIndex);
    void                resetHistory(int aStep);
    int                 timeShift()    { return mTimeShift;}
    int                 timeIndex()    { return mTimeIndex;}
private :
    template <typename T>
    QVariantList&       toVariantList( const QList<T> &list );

    FbsfDataExchange*   pPublicData; // the public data
    int                 mTimeIndex;  // vector current time index
    int                 mTimeShift;  // shift time each step (time dependant vector)
    bool                mInitial;    // flag for initial recording

    void*               pHistoryVector; // pointer to history vector
    void*               pFuturVector;   // pointer to futur vector

    QVariantList        dataList;

};

#endif // FBSFPUBLICDATARECORDER_H
