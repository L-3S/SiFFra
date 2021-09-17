#ifndef FBSFREPLAYFILEAPI_H
#define FBSFREPLAYFILEAPI_H

#include <QtCore/qglobal.h>
#include <QVariant>

#if defined(FBSFREPLAYFILEAPI_LIBRARY)
#  define FBSFREPLAYFILEAPISHARED_EXPORT Q_DECL_EXPORT
#include "Fbsfglobal.h"
#else
#  define FBSFREPLAYFILEAPISHARED_EXPORT Q_DECL_IMPORT
#endif

class DataInfo;
class FbsfDataExchange;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @class FbsfReplayFileAPI
/// Class for reading replay file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSFREPLAYFILEAPISHARED_EXPORT FbsfReplayFileAPI
{

public:
    FbsfReplayFileAPI(QString aFile);
    ~FbsfReplayFileAPI();

    // API methods
    void                getDataList(QList<QString> & aList);
    int                 getReplayDuration() { return mReplayDuration;}
    bool                isStandardMode();

private :
    QMap<QString,DataInfo> mDataNameList;
    int                    mReplayDuration;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @class DataInfo
/// Class for getting information and values
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class FBSFREPLAYFILEAPISHARED_EXPORT DataInfo
{
    public :
    enum DataTypes
    {
        cNoType         = '?',
        cInteger        = 'I',
        cReal           = 'R'
    } ;

    enum DataClass
    {
        cScalar       = 'S',
        cVector       = 'V'
    } ;

    DataInfo(QString aName);
    // Informations getters
    QString                 Name() const;
    QString                 Producer() const;
    DataClass               Class() const;
    QString                 ClassString() const;
    QString                 TypeString() const;
    DataTypes               Type() const;
    QString                 Description() const;
    QString                 Unit() const;
    int                     Size() const;
    // Time depend informations getters
    int                     TimeShift() const;
    int                     TimeIndex() const;
    // values getters
    const QVariantList &    Values() const;
    const QVariantList &    VectorValues(int aIndex) const;
    const QVariantList &    TimeDependValues(int aStep) const;


private :
    FbsfDataExchange *              mPublicAddress ;
};
#endif // FBSFREPLAYFILEAPI_H
