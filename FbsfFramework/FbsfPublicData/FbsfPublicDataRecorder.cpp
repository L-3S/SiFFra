#include "FbsfPublicData.h"
#include <QDataStream>
#define REPLAY_TIME_DEPEND

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Recorder CTOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfPublicDataRecorder::FbsfPublicDataRecorder(FbsfDataExchange *aData,
                                               int aTimeIndex, int aTimeShift)
    : pPublicData (aData)
    , mTimeIndex(aTimeIndex)
    , mTimeShift(aTimeShift)
    , mInitial(false) // initial past values are not significants
    , pHistoryVector(nullptr)
    , pFuturVector(nullptr)
{
    switch (pPublicData->Class())
    {
    case cScalar :
        //~~~~~~~~~~ scalar data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (pPublicData->Type() == cInteger)
            pHistoryVector=new QList<int>();
        else
            pHistoryVector=new QList<qreal>();
        break;
    case cVector :
        if(mTimeShift>0)
        {   //~~~~~~~~~~~~~ time dependant vector ~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
            {
                pHistoryVector=new QList<int>();
#ifdef REPLAY_TIME_DEPEND
                pFuturVector=new QList<int>();
#endif
            }
            else
            {
                pHistoryVector=new QList<qreal>();
#ifdef REPLAY_TIME_DEPEND
                pFuturVector=new QList<qreal>();
#endif
            }
        }
        else
        {   //~~~~~~~~~~~~~ data vector ~~~~~~~~~~~~~~~~~~~
            // One vector per Item
            if (pPublicData->Type() == cInteger)
                pHistoryVector = new QVector<QList<int>>(pPublicData->size());
            else
                pHistoryVector = new QVector<QList<qreal>>(pPublicData->size());
        }
        break;
    default : break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfPublicDataRecorder::~FbsfPublicDataRecorder()
{
    if (pHistoryVector) delete pHistoryVector;
    if (pFuturVector)   delete pFuturVector;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Copy constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfPublicDataRecorder::FbsfPublicDataRecorder(const FbsfPublicDataRecorder& rec)
{
    // disable copy
    Q_UNUSED(rec);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Record public data : scalar or vector (value or time dependant)
/// Case replay mode : history data is pushed back to public data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPublicDataRecorder::record(int aStep)
{
    if (pPublicData==nullptr) return;

    switch (pPublicData->Class())
    {
    case cScalar :
        //~~~~~~~~~~ scalar data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (pPublicData->Type() == cInteger)
        {
            QList<int>* pVector=static_cast<QList<int>*>(pHistoryVector);
            if (pPublicData->replayed())
            {   // Replay mode
                if (aStep < pVector->size() )
                    pPublicData->setIntValue((*pVector)[aStep]);
            }
            else
            {   // Compute mode
                pVector->append(pPublicData->m_value.mInteger);
                // check maximum recording size
                if(pVector->length() >= FbsfDataExchange::recorderSize())
                    pVector->removeFirst();
            }
        }
        else // qreal
        {
            QList<qreal>* pVector=static_cast<QList<qreal>*>(pHistoryVector);
            if (pPublicData->replayed())
            {   // Replay mode
                if (aStep < pVector->size() )
                    pPublicData->setRealValue((*pVector)[aStep]);
            }
            else
            {   // Compute mode
                pVector->append(pPublicData->m_value.mReal);
                // check maximum recording size
                if(pVector->length()>= FbsfDataExchange::recorderSize())
                    pVector->removeFirst();
            }
        }
        break;
    case cVector :
        if(mTimeShift>0)
        {   //~~~~~~~~~~~~~~~ time dependant vector ~~~~~~~~~~~~~~~~~~~~~~~
#if INITIAL_CASE
            if (mInitial)// recorder size is assumed greater than vector size
            {
                mInitial=false;
                if (pPublicData->Type() == FbsfDataExchange::cInteger)
                {
                    QList<int>* pVector=static_cast<QList<int>*>(pHistoryVector);
                    for(int i=0;i < mTimeIndex;i++)
                        pVector->append(pPublicData->p_data[i].mInteger);
                }
                else
                {
                    QList<qreal>* pVector=static_cast<QList<qreal>*>(pHistoryVector);
                    for(int i=0;i < mTimeIndex;i++)
                        pVector->append(pPublicData->p_data[i].mReal);
                }
            }
            else
#endif
            {
                if (pPublicData->Type() == cInteger)
                {
#ifdef REPLAY_TIME_DEPEND
                    if (pPublicData->replayed())
                    {   // Replay mode
                        QList<int>& pVector=*static_cast<QList<int>*>(pHistoryVector);
                        if (aStep < pVector.size() )
                        {
                            QVector<int> pData(pPublicData->size(),0);
                            // Get the past part
                            for(int i=0;i < mTimeShift;i++)
                                pData[mTimeIndex - mTimeShift + i]= pVector[aStep*mTimeShift+i];
                            // Get the futur part
                            QList<int>& pFVector=*static_cast<QList<int>*>(pFuturVector);
                            int FutureSize = pPublicData->size() - mTimeIndex;
                            for(int i=0;i < FutureSize;i++)
                                pData[mTimeIndex+i]= pFVector[aStep*FutureSize+i];
                            pPublicData->setData(&pData);
                        }
                    }
                    else
#endif // REPLAY_TIME_DEPEND
                    {   // Compute mode
                        QList<int>* pVector=static_cast<QList<int>*>(pHistoryVector);
                        bool limitSize=false;
                        // Record the "timeshift" values from public data (Past)
                        for(int i=mTimeIndex-mTimeShift;i < mTimeIndex;i++)
                        {
                            pVector->append(pPublicData->p_data[i].mInteger);
                            limitSize=(pVector->length()>=FbsfDataExchange::recorderSize());
                            if(limitSize)pVector->removeFirst();
                        }
#ifdef REPLAY_TIME_DEPEND
                        // Record the futur part of the vector
                        QList<int>* pFVector=static_cast<QList<int>*>(pFuturVector);
                        for(int i=mTimeIndex;i < pPublicData->size();i++)
                        {
                            pFVector->append(pPublicData->p_data[i].mInteger);
                            if(limitSize) pFVector->removeFirst();
                        }
#endif // // REPLAY_TIME_DEPEND
                    }// end modes
                }
                else
                {
#ifdef REPLAY_TIME_DEPEND
                    if (pPublicData->replayed())
                    {   // Replay mode
                        QList<qreal>& pVector=*static_cast<QList<qreal>*>(pHistoryVector);
                        if (aStep < pVector.size() )
                        {
                            QVector<real> pData(pPublicData->size(),0);

                            for(int i=0;i < mTimeShift;i++)
                                pData[mTimeIndex - mTimeShift + i]= pVector[aStep*mTimeShift+i];
                            // Get the futur part
                            QList<qreal>& pFVector=*static_cast<QList<qreal>*>(pFuturVector);
                            int FutureSize = pPublicData->size() - mTimeIndex;
                            for(int i=0;i < FutureSize;i++)
                                pData[mTimeIndex+i]= pFVector[aStep*FutureSize+i];
                            pPublicData->setData(&pData);
                        }
                    }
                    else
#endif
                    {   // Compute mode
                        QList<qreal>* pVector=static_cast<QList<qreal>*>(pHistoryVector);
                        bool limitSize=false;
                        for(int i=mTimeIndex-mTimeShift;i < mTimeIndex;i++)
                        {
                            pVector->append(pPublicData->p_data[i].mReal);
                            limitSize=(pVector->length()>=FbsfDataExchange::recorderSize());
                            if(limitSize)pVector->removeFirst();
                        }
#ifdef REPLAY_TIME_DEPEND
                        // record futur part of the vector
                        QList<qreal>* pFVector=static_cast<QList<qreal>*>(pFuturVector);
                        for(int i=mTimeIndex;i < pPublicData->size();i++)
                        {
                            pFVector->append(pPublicData->p_data[i].mReal);
                            if(limitSize) pFVector->removeFirst();
                        }
#endif
                    }// end modes
                }
            }
        }
        else
        {   //~~~~~~~~~~~~~~~ data vector ~~~~~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
            {
                QVector<QList<int>>* pVector=static_cast<QVector<QList<int>>*>(pHistoryVector);

                if (pPublicData->replayed())
                {
                    if (pVector->length() >0 && aStep < (*pVector)[0].size() )
                    {
                        QVector<int> vector; // step vector data
                        for (int i=0 ; i < pVector->length() ; i++)
                            vector.append((*pVector)[i][aStep]);
                        pPublicData->setData(&vector);
                    }
                }
                else
                {
                    if(pPublicData->p_data!=nullptr)
                        for (int i=0 ; i < pVector->length() ; i++)
                        {
                            (*pVector)[i].append(pPublicData->p_data[i].mInteger);
                        }
                }
            }
            else
            {
                QVector<QList<qreal>>* pVector=static_cast<QVector<QList<qreal>>*>(pHistoryVector);

                if (pPublicData->replayed())
                {
                    if (aStep < (*pVector)[0].size() )
                    {
                        QVector<real> vector; // step vector data
                        for (int i=0 ; i < pVector->length() ; i++)
                            vector.append((*pVector)[i][aStep]);

                        pPublicData->setData(&vector);
                    }
                }
                else
                {
                    if(pPublicData->p_data!=nullptr)
                        for (int i=0 ; i < pVector->length() ; i++)
                        {
                            (*pVector)[i].append(pPublicData->p_data[i].mReal);
                        }

                }
            }
        }
        break;
    default : break;
    }
}

void FbsfPublicDataRecorder::setBacktrack(int aStep)
{
    if (pPublicData==nullptr) return;

    switch (pPublicData->Class())
    {
    case cScalar :
        //~~~~~~~~~~ scalar data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (pPublicData->Type() == cInteger)
        {
            if (aStep < pPublicData->backtrackHistory().size())
                pPublicData->setIntValue((pPublicData->backtrackHistory())[aStep].toInt(), true);
        }
        else // qreal
        {
            if (aStep < pPublicData->backtrackHistory().size())
                pPublicData->setRealValue((pPublicData->backtrackHistory())[aStep].toFloat(), true);
        }
        break;
    case cVector :
        if(mTimeShift>0) {
            int FutureSize = pPublicData->size() - mTimeIndex;
            int PastSize = pPublicData->size() - FutureSize;

            if (pPublicData->Type() == cInteger) {
                QList<int> backVector;
                QVector<int> pData(pPublicData->size(),0);

                for (auto it : pPublicData->backtrackHistory())
                    backVector.push_back(it.toInt());
                if (aStep * mTimeShift + FutureSize >= backVector.size()) {
                    int max = (backVector.size() - FutureSize) / mTimeShift - 1;
                    int add = aStep - max;

                    for(int i=0 ;i < PastSize;i++)
                        if (max*mTimeShift+add+i >= PastSize - mTimeShift)
                            pData[i]= backVector[max*mTimeShift+add+i - (PastSize-mTimeShift)];
                    for(int i=0;i < FutureSize && max*mTimeShift+PastSize+i+add - (PastSize-mTimeShift) < backVector.size();i++)
                        pData[PastSize+i]= backVector[max*mTimeShift+PastSize+i+add - (PastSize-mTimeShift)];
                    pPublicData->setData(&pData, true);
                } else {
                    for(int i=0 ;i < PastSize;i++)
                        if (aStep*mTimeShift+i >= PastSize - mTimeShift)
                            pData[i]= backVector[aStep*mTimeShift+i - (PastSize-mTimeShift)];
                    for(int i=0;i < FutureSize;i++)
                        pData[PastSize+i]= backVector[aStep*mTimeShift+PastSize+i - (PastSize-mTimeShift)];
                    pPublicData->setData(&pData, true);
                }
            } else {
                QList<qreal> backVector;
                QVector<real> pData(pPublicData->size(),0);

                for (auto it : pPublicData->backtrackHistory())
                    backVector.push_back(it.toFloat());
                if (aStep * mTimeShift + FutureSize >= backVector.size()) {
                    int max = (backVector.size() - FutureSize) / mTimeShift - 1;
                    int add = aStep - max;

                    for(int i=0 ;i < PastSize;i++)
                        if (max*mTimeShift+add+i >= PastSize - mTimeShift)
                            pData[i]= backVector[max*mTimeShift+add+i - (PastSize-mTimeShift)];
                    for(int i=0;i < FutureSize && max*mTimeShift+PastSize+i+add - (PastSize-mTimeShift) < backVector.size();i++)
                        pData[PastSize+i]= backVector[max*mTimeShift+PastSize+i+add - (PastSize-mTimeShift)];
                    pPublicData->setData(&pData, true);
                } else {
                    for(int i=0 ;i < PastSize;i++)
                        if (aStep*mTimeShift+i >= PastSize - mTimeShift)
                            pData[i]= backVector[aStep*mTimeShift+i - (PastSize-mTimeShift)];
                    for(int i=0;i < FutureSize;i++)
                        pData[PastSize+i]= backVector[aStep*mTimeShift+PastSize+i - (PastSize-mTimeShift)];
                    pPublicData->setData(&pData, true);
                }
            }
        } else {   //~~~~~~~~~~~~~~~ data vector ~~~~~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
            {
                QVector<QVariant> tmpVector= pPublicData->backtrackHistory().toVector();
                QVector<QList<int>> pVector;
                QList<int> tmp;
                for (auto it : tmpVector) {
                    tmp.clear();
                    for (auto var : it.toList())
                        tmp.push_back(var.toInt());
                    pVector.push_back(tmp);
                }
                if (aStep < (pVector)[0].size() )
                {
                    QVector<int> vector(pVector.size(),0); // step vector data
                    for (int i=0 ; i < pVector.length() ; i++) {
                        vector[i] = ((pVector)[i][aStep]);
                    }
                    pPublicData->setData(&vector, true);
                }
            }
            else
            {
                QVector<QVariant> tmpVector= pPublicData->backtrackHistory().toVector();
                QVector<QList<qreal>> pVector;
                QList<qreal> tmp;
                for (auto it : tmpVector) {
                    tmp.clear();
                    for (auto var : it.toList())
                        tmp.push_back(var.toFloat());
                    pVector.push_back(tmp);
                }
                if (aStep < (pVector)[0].size())
                {
                    QVector<real> vector(pVector.size(),0); // step vector data
                    for (int i=0 ; i < pVector.length() ; i++)
                        vector[i] = ((pVector)[i][aStep]);
                    pPublicData->setData(&vector, true);
                }
            }
        }
        break;
    default : break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Accessor for getting data history
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariantList& FbsfPublicDataRecorder::history()
{
    if (pPublicData->Type() == cInteger)
        return toVariantList(*(static_cast<QList<int>*>(pHistoryVector)));
    else
        return toVariantList(*(static_cast<QList<qreal>*>(pHistoryVector)));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Accessor for getting data futur
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariantList& FbsfPublicDataRecorder::historywithfutur()
{
    dataList.clear();
    history();// fill dataList
    if (pPublicData->p_data!=nullptr)
    {
        if (pPublicData->Type() == cInteger)
            for(int i=mTimeIndex;i < pPublicData->size();i++)
                dataList << pPublicData->p_data[i].mInteger;
        else
            for(int i=mTimeIndex;i < pPublicData->size();i++)
                dataList << pPublicData->p_data[i].mReal;
    }
    return dataList;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Accessor for getting indexed data history for a vector
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QVariantList& FbsfPublicDataRecorder::history(int aIndex)
{
    if (pPublicData->Type() == cInteger)
        return toVariantList((*static_cast<QVector<QList<int>>*>(pHistoryVector))[aIndex]);
    else
        return toVariantList((*static_cast<QVector<QList<qreal>>*>(pHistoryVector))[aIndex]);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Accessor for getting history size
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int FbsfPublicDataRecorder::historySize()
{
    switch (pPublicData->Class())
    {
    case cScalar :
        //~~~~~~~~~~~~~~ scalar data~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (pPublicData->Type() == cInteger)
            return (static_cast<QList<int>*>(pHistoryVector))->size();
        else
            return (static_cast<QList<qreal>*>(pHistoryVector))->size();
        break;
    case cVector :

        if(mTimeShift>0)
        {   //~~~~~~~~~~~~~~~~~ time dependant vector~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
                return (static_cast<QList<int>*>(pHistoryVector))->size();
            else
                return (static_cast<QList<qreal>*>(pHistoryVector))->size();
            break;
        }
        else
        {   //~~~~~~~~~~~~~~~~~ data vector~~~~~~~~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
                return (static_cast<QVector<QList<int>>*>(pHistoryVector))->size();
            else
                return (static_cast<QVector<QList<qreal>>*>(pHistoryVector))->size();
            break;
        }
    default : return 0;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Reset history from aStep to end
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPublicDataRecorder::resetHistory(int aStep)
{
    switch (pPublicData->Class())
    {
    case cScalar :
        //~~~~~~~~~~~~~~~~ scalar data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (pPublicData->Type() == cInteger)
        {
            QList<int>* pVector=static_cast<QList<int>*>(pHistoryVector);
            if (!pVector->isEmpty())
                while (pVector->length()> aStep+1) {pVector->removeLast();}
        }
        else
        {
            QList<qreal>* pVector=static_cast<QList<qreal>*>(pHistoryVector);
            if (!pVector->isEmpty())
                while (pVector->length()> aStep+1) {pVector->removeLast();}
        }
        break;
    case cVector :
        if(pPublicData->timeShift()>0)
        {   //~~~~~~~~~~~~~ time dependant vector ~~~~~~~~~~~~~~~~~~~
            // TODO timeshift vlues to remove up to aStep
            if (pPublicData->Type() == cInteger)
            {
                QList<int>* pVector=static_cast<QList<int>*>(pHistoryVector);
                if (!pVector->isEmpty())
                    while (pVector->length()> (aStep+1)*pPublicData->timeShift() )
                    {pVector->removeLast();}
                // reset full futur
                QList<int>* pFVector=static_cast<QList<int>*>(pFuturVector);
                pFVector->clear();
            }
            else
            {
                QList<qreal>* pVector=static_cast<QList<qreal>*>(pHistoryVector);
                if (!pVector->isEmpty())
                    while (pVector->length()> (aStep+1)*pPublicData->timeShift() )
                    {pVector->removeLast();}
                // reset full futur
                QList<qreal>* pFVector=static_cast<QList<qreal>*>(pFuturVector);

                pFVector->clear();
            }
        }
        else
        {   //~~~~~~~~~~~~~ data vector ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
            {
                QVector<QList<int>>* pVector=static_cast<QVector<QList<int>>*>(pHistoryVector);
                for (int i=0 ; i < pVector->length() ; i++)
                    if (!(*pVector)[i].isEmpty())
                        while ((*pVector)[i].length()>aStep+1) {(*pVector)[i].removeLast();}
            }
            else
            {
                QVector<QList<qreal>>* pVector=static_cast<QVector<QList<qreal>>*>(pHistoryVector);
                for (int i=0 ; i < pVector->length() ; i++)
                    if (!(*pVector)[i].isEmpty())
                        while ((*pVector)[i].length()>aStep+1) {(*pVector)[i].removeLast();}
            }
        }
        break;
    default : break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Converter for QList to QVariantList
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <typename T>
QVariantList& FbsfPublicDataRecorder::toVariantList( const QList<T> &list )
{
    dataList.clear();
    if (pPublicData->replayed())
    {   // Replay mode : limit the dataList size to current step number
        if (&list != 0)
            for(int i=0;i < list.size() && i<= pPublicData->ReplayStep(); i++)
                dataList << list[i];
    }
    else
    {   // Compute mode
        if (&list != nullptr) foreach( const T &item, list ) dataList << item;
    }
    return dataList;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Serialize public data to stream : scalar or vector (value or time dependant)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPublicDataRecorder::serialize(QDataStream& aStream)
{
    switch (pPublicData->Class())
    {
    case cScalar :
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (pPublicData->Type() == cInteger)
            aStream << *(static_cast<QList<int>*>(pHistoryVector));
        else
            aStream << *(static_cast<QList<qreal>*>(pHistoryVector));
        break;
    case cVector :
#ifdef REPLAY_TIME_DEPEND
        if(mTimeShift>0)
        {
            //~~~~~~~~~~~~~ time dependant vector ~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
            {
                aStream << *(static_cast<QList<int>*>(pHistoryVector));
                aStream << *(static_cast<QList<int>*>(pFuturVector));
            }
            else
            {
                aStream << *(static_cast<QList<qreal>*>(pHistoryVector));
                aStream << *(static_cast<QList<qreal>*>(pFuturVector));
            }
        }
        else
#endif // REPLAY_TIME_DEPEND
        {
            //~~~~~~~~~~~~~ data vector ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
                aStream << *(static_cast<QVector<QList<int>>*>(pHistoryVector));
            else
                aStream << *(static_cast<QVector<QList<qreal>>*>(pHistoryVector));
        }
        break;

    default : break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Deserialize public data from stream : scalar or vector (value or time dependant)
/// Replay mode : history data is pushed back to the public data pool
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfPublicDataRecorder::deserialize(QDataStream& aStream)
{
    switch (pPublicData->Class())
    {
    case cScalar :
        //~~~~~~~~~~~~~~~~ scalar data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if (pPublicData->Type() == cInteger)
            aStream >> *(static_cast<QList<int>*>(pHistoryVector));
        else
            aStream >> *(static_cast<QList<qreal>*>(pHistoryVector));
        break;
    case cVector :
#ifdef REPLAY_TIME_DEPEND
        if(pPublicData->timeShift()>0)
        {   //~~~~~~~~~~~~~ time dependant vector ~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
            {
                aStream >> *(static_cast<QList<int>*>(pHistoryVector));
                aStream >> *(static_cast<QList<int>*>(pFuturVector));
            }
            else
            {
                aStream >> *(static_cast<QList<qreal>*>(pHistoryVector));
                aStream >> *(static_cast<QList<qreal>*>(pFuturVector));
            }
        }
        else
#endif // REPLAY_TIME_DEPEND
        {   //~~~~~~~~~~~~~ data vector ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if (pPublicData->Type() == cInteger)
                aStream >> *(static_cast<QVector<QList<int>>*>(pHistoryVector));
            else
                aStream >> *(static_cast<QVector<QList<qreal>>*>(pHistoryVector));
        }
        break;
    default : break;
    }
}
