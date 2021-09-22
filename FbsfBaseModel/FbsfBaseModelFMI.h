#ifndef FBSFBASEMODELFMI_H
#define FBSFBASEMODELFMI_H

#include "FbsfBaseModel.h"

class ModelDescription;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Class for FMI public variables
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_BASEMODEL_EXPORT FmiVariable
{
public :
    FmiVariable()
        : mName("")
        , mCausality("")
        , mVariability("")
        , mUnit("")
        , mDescription("")
    {}

    QString             name()              {return mName;}
    void                name(QString aName) { mName=aName;}

    QString             causality()         {return mCausality;}
    void                causality(QString aCausality) { mCausality=aCausality;}

    QString             variability()       {return mVariability;}
    void                variability(QString aVariability)   {mVariability=aVariability;}

    QString             unit()              {return mUnit;}
    void                unit(QString aUnit) {mUnit=aUnit;}

    QString             description()           {return mDescription;}
    void                description(QString aDescription)   {mDescription=aDescription;}
    //value accessors
    int*                pIntValue()         {return &mValue.mInt;}
    real*               pRealValue()        {return &mValue.mReal;}
    bool                Bool()              {return mValue.mInt==1?true:false;}
    int                 Int()               {return mValue.mInt;}
    real                Real()              {return mValue.mReal;}
    void                value(bool aValue)  {mValue.mInt=aValue;}
    void                value(int aValue)   {mValue.mInt=aValue;}
    void                value(real aValue)  {mValue.mReal=aValue;}
private :
    QString             mName;
    QString             mCausality;
    QString             mVariability;
    QString             mUnit;
    QString             mDescription;    

    union{
        bool    mBool;
        int     mInt;
        real    mReal;
    }
    mValue;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Class for FMI public parameters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class FBSF_BASEMODEL_EXPORT FBSFBaseModelFMI
        : public FBSFBaseModel
{
public:
    FBSFBaseModelFMI();
    ~FBSFBaseModelFMI();
    int     parseModelDescription(QString aFile);
    void    printModelDescription();
    void    declareFmiParameters();
    FmiVariable& parameter(QString aName)
    {
        if(yParameters.contains(aName))
        {
            //return a reference
            return *yParameters[aName];
        }
        else
        {
            QString msg(name()+" : Unknown parameter " + aName);
            qFatal(msg.toStdString().c_str());

            return invalidFmiVariable;
        }
    }
    virtual QMap<QString, ParamProperties> getParamList() override {return {};}

private :
    ModelDescription* mModelDescription;

    QHash<QString,FmiVariable*> yParameters;    // List of fmi parameters

    FmiVariable invalidFmiVariable;

};

#endif // FBSFBASEMODELFMI_H
