#ifndef ITEMPARAMS_H
#define ITEMPARAMS_H
#include <QObject>
#include <QVariant>
#include <QDate>
#include <QDebug>
#include "ParamProperties.h" // include from FBSF
#include "ItemProperties.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class for data descriptor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ParamValue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString key READ key CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(bool optional READ optional CONSTANT)
    Q_PROPERTY(bool mandatory READ mandatory CONSTANT)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(QVariant minStrict READ minStrict CONSTANT)
    Q_PROPERTY(QVariant maxStrict READ maxStrict CONSTANT)
    Q_PROPERTY(QVariant minWarn READ minWarn CONSTANT)
    Q_PROPERTY(QVariant maxWarn READ maxWarn CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(QDate isoDate READ isoDate CONSTANT)

    Q_PROPERTY(QString error READ error CONSTANT)
    Q_PROPERTY(bool hasError READ hasError NOTIFY hasErrorChanged)

public:
    // Default constructor
    explicit ParamValue(QObject *parent = nullptr)
        : QObject(parent)
        , mKey(), mType(), mOptional(), mMandatory(), mValue()
        , mUnit(), mMinStrict(), mMaxStrict()
        , mMinWarn(), mMaxWarn(),mDescription()
        , mIndex(0)
    {
    }
    /// Constructor for QVariant => Text
    ParamValue(const QString& aKey,const QVariant& aValue,const ParamProperties& aProps)
        : mKey(aKey), mType(),
          mOptional(aProps.mP_qual==Param_quality::cOptional),
          mMandatory(aProps.mP_qual==Param_quality::cMandatory),
          mValue(aValue),mUnit(aProps.mUnit),
          mMinStrict(aProps.mMin_strict), mMaxStrict(aProps.mMax_strict),
          mMinWarn(aProps.mMin_warn), mMaxWarn(aProps.mMax_warn),
          mDescription(aProps.mDescription),
          mIndex(0),
          mDefaultType(aProps.mDefault.type()),
          mDefaultValue(aProps.mDefault)
    {
        switch (aProps.mP_type)
        {
        case Param_type::cStr   : mType=typeString;break;
        case Param_type::cDbl   : mType=typeNumber;break;
        case Param_type::cInt   : mType=typeInt;break;
        case Param_type::cBool  : mType=typeBool;break;
        case Param_type::cCheckable  : mType=typeCheckable;break;
        case Param_type::cPath  : mType=typePath;break;
        case Param_type::cFilePath  : mType=typeFilePath;break;
        case Param_type::cDateAndTime : mType=typeDateAndTime;break;
        case Param_type::cDateAndTimeUTC : mType=typeDateAndTimeUTC;break;
        case Param_type::cDate  : {
            mType=typeDate;
            //QString format=dateFormat.isEmpty()?defaultDateFormat:dateFormat;
            mUnit=dateFormat;//format;// static member
            // set date to ISO Date for JavaScript
            mIsoDate=QDate::fromString(mValue.toString(),"yyyy-MM-dd");
        }
            break;
        case Param_type::cTime          : {
            mType=typeTime;
            //QString format=dateFormat.isEmpty()?defaultTimeFormat:dateFormat;
            mUnit=timeFormat;//format;// static member
            // set time to ISO Time for JavaScript
            mIsoTime=QTime::fromString(mValue.toString(),"hh:mm:ss");
        }
            break;
        case Param_type::cEnumString    : mType=typeStringList;break;
        case Param_type::cEnumInt       : mType=typeStringList;break;
        //case Param_type::cChoiceList    : mType=typeChoiceList;break;
        }
    }
    /// Constructor for QStringList => Combobox (index is mandatory)
    ParamValue(const QString& aKey,const QVector<QVariant>& aValue,
               const ParamProperties& aProps,const QVariant& currentText)
        : mKey(aKey),
          //mType(aProps.mP_type==Param_type::cChoiceList?typeChoiceList:typeStringList),
          mType(typeStringList),
          mOptional(aProps.mP_qual==Param_quality::cOptional),
          mMandatory(aProps.mP_qual==Param_quality::cMandatory),
          mValue(aValue.toList()),mUnit(aProps.mUnit),
          mMinStrict(aProps.mMin_strict), mMaxStrict(aProps.mMax_strict),
          mMinWarn(aProps.mMin_warn), mMaxWarn(aProps.mMax_warn),
          mDescription(aProps.mDescription),
          mIndex(-1),mDefaultType(QVariant::String)
    {
        if(currentText.toString().isEmpty())
        {
            mIndex=-1;
        }
        else
        {
            // check index for current item
            for(int i=0;i<aValue.size();i++)
            {
                if(mValue.toList()[i]==currentText)
                {
                    mIndex=i;
                    break;
                }
            }
        }
        mDefaultValue=aProps.mDefault;
    }
    // Copy constructor and assignement
    ParamValue(const ParamValue &other): QObject(other.parent()) {*this=other;}
    ParamValue& operator=(const ParamValue &other){return *other.clone(); }
    /// Clone pointed data (usefull for default value)
    ParamValue* clone() const {
        ParamValue* newValue = new ParamValue();
        newValue->mKey=mKey;
        newValue->mType=mType;
        newValue->mOptional=mOptional;
        newValue->mMandatory=mMandatory;
        newValue->mValue=mValue;
        newValue->mUnit=mUnit;
        newValue->mMinStrict=mMinStrict;
        newValue->mMaxStrict=mMaxStrict;
        newValue->mMinWarn=mMinWarn;
        newValue->mMaxWarn=mMaxWarn;
        newValue->mDescription=mDescription;
        newValue->mIndex=mIndex;
        newValue->mDefaultType=mDefaultType;
        newValue->mDefaultValue=mDefaultValue;

        return newValue;
    }
    // Value type for QML
    static QString typeInt;
    static QString typeNumber;
    static QString typeString;
    static QString typeBool;
    static QString typeCheckable;
    static QString typeStringList;
    static QString typeIntList;
    static QString typePath;
    static QString typeFilePath;
    static QString typeDateAndTime;
    static QString typeDateAndTimeUTC;
    static QString typeDate;
    static QString typeTime;
//    static QString typeChoiceList;
    static QString dateFormat;
    static QString timeFormat;

    // accessors
    const QString&  key()           const {return mKey;}
    void            key(QString& aKey) {mKey=aKey;}
    const QString&  type()          const {return mType;}
    QVariant::Type  defaultType() const {return mDefaultType;}
    bool            optional()      const {return mOptional;}
    bool            mandatory()    const {return mMandatory;}
    const QVariant& value()         const {return mValue;}
    const QString&  unit()          const {return mUnit;}
    const QVariant& minStrict()     const {return mMinStrict;}
    const QVariant& maxStrict()     const {return mMaxStrict;}
    const QVariant& minWarn()       const {return mMinWarn;}
    const QVariant& maxWarn()       const {return mMaxWarn;}
    const QString&  description()   const {return mDescription;}
    int             index()         const {return mIndex;}
    const QDate&    isoDate()       const {return mIsoDate;}
    const QTime&    isoTime()       const {return mIsoTime;}
    const QString&  error()         const {return mError;}
    void            error(QString msg)  {mError=msg;}

    QString         currentText() const
    {
        if(mType==typeStringList )
            return mIndex>=0 && mIndex < mValue.toStringList().size()?
                        mValue.toStringList()[mIndex]:"";
        else
            return "???";
    }
    // helpers
    QVariant getValue() const
    {
        return (mType==typeStringList?currentText():mValue);
    }
    bool isModified()
    {
        return(mMandatory || getValue()!=mDefaultValue);
    }
    bool            isInvalid() ;
    bool            hasError() const {return mHasError; }
    void            hasError(bool aFlag) {mHasError=aFlag;emit hasErrorChanged(aFlag);}
public slots:
    // called from QML textfield
    void setValue(const QVariant& aValue)
    {
        if(mValue==aValue) return;
        mValue=aValue;
        if(mType==typeDate)
            mIsoDate=QDate::fromString(mValue.toString(),mUnit);
        else if(mType==typeTime)
            mIsoTime=QTime::fromString(mValue.toString(),mUnit);
        // check validity
        bool status=isInvalid();
        hasError(status);// emit signal to QML
    }
    // called from QML combobox
    void setIndex(int aIndex)
    {
        if(mIndex==aIndex) return;
        mIndex=aIndex;
        hasError(false); // we assume value as valid
    }
    // We use unit as date format
    void setUnit(QString aUnit)
    {
        if(mUnit==aUnit) return;
        mUnit=aUnit;
        emit unitChanged(mUnit);
        hasError(false); // we assume value as valid
    }

signals:
    // signal change to parent model
    void valueChanged(QString aKey, QString aName,QVariant aValue);
    void indexChanged(int aIndex);
    void unitChanged(QString aUnit);
    void hasErrorChanged(bool aStatus);

private :
    QString     mKey;
    QString     mType;
    bool        mOptional;
    bool        mMandatory;
    QVariant    mValue;
    QString     mUnit;
    QVariant    mMinStrict;
    QVariant    mMaxStrict;
    QVariant    mMinWarn;
    QVariant    mMaxWarn;
    QString     mDescription;
    int         mIndex;     // index if type is list
    QDate       mIsoDate;
    QTime       mIsoTime;
    QString     mError;

    QVariant::Type  mDefaultType;
    QVariant        mDefaultValue;

    bool            mHasError=false;

};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ParamList : public QList<QObject*>
{
public :
    // Default constructor
    explicit ParamList():  QList<QObject*>(){}
    // Destructor
    virtual ~ParamList(){qDeleteAll(begin(),end());clear();}
    // Copy constructor
    ParamList(const ParamList &other)
    {
        for (int i=0;i< other.size();i++) append(other[i].clone());
    }
    // Assignement operator
    ParamList& operator=(const ParamList &other)
    {
        for (int i=0;i< other.size();i++) append(other[i].clone());
        return *this;
    }
    // Subscript operator
    ParamValue& operator[](int aIndex) const
    {
        if(aIndex>= size()) aIndex=0; // set to 0 fore safe
        return *( dynamic_cast<ParamValue*>(at(aIndex)));// down cast
    }
    // get value by key
    const QVariant getValue(QVariant aKey) const
    {
        for (int i=0;i< size();i++)
        {
            ParamValue& data=*( dynamic_cast<ParamValue*>(at(i)));// down cast
            if(data.key()==aKey.toString())
            {
                return data.getValue();
            }
        }
        return QVariant();
    }

    //
    /*!
     * \brief setValue: set value (arguments = key ; value)
     * \param aKey
     * \param aVal
     * When the underlying type is enumerated, the setter fixes the underlying index
     */
    void setValue(const QString aKey, const QVariant& aVal) {
        for (int i=0;i< size();i++)
        {
            ParamValue& data=*( dynamic_cast<ParamValue*>(at(i)));// down cast
            if(data.key()==aKey)
            {

                if (data.type() == ParamValue::typeStringList) {
                    if(aVal.toString().isEmpty())
                        data.setIndex(-1);
                    else
                    {
                        // check index for current item; -1 if not found
                        data.setIndex(data.value().toStringList().indexOf(aVal.toString()));
                    }
                }
                else      data.setValue(aVal);
            }
        }
    }

    // row count accessor
    int dataCount() const { return size();}

    // add value data (mandatory first)
    void addData(const QString& aKey,const QVariant& aValue,const ParamProperties& aProps)
    {
        if(aProps.mP_qual==Param_quality::cMandatory)
            prepend(new ParamValue(aKey,aValue,aProps));
        else
            append(new ParamValue(aKey,aValue,aProps));
    }
    // add list data (mandatory first)
    void addData(const QString& aKey,const QVector<QVariant>& aValue,
                 const ParamProperties& aProps,const QVariant& currentText)
    {
        if(aProps.mP_qual==Param_quality::cMandatory)
            prepend(new ParamValue(aKey,aValue,aProps,currentText));
        else
            append(new ParamValue(aKey,aValue,aProps,currentText));
    }
    //~~~~~~~~~~~~~~~~~~~~ param updaters for loaded ~~~~~~~~~~~~~~~~~~~~~~~~
    void setParamsScalar(QMap<QString,QString>& aXmlParamList,
                         const QMap<QString, ParamProperties>::const_iterator it);
    void setParamsEnumList(QMap<QString,QString>& aXmlParamList,
                           const QMap<QString, ParamProperties>::const_iterator it);
    void setParamsChoiceList(QMap<QString,QString>& aXmlParamList,
                           const QMap<QString, ParamProperties>::const_iterator it);
    void setItemParams(QMap<QString,QString>& aXmlParamList,
                       const QMap<QString, ParamProperties> &aParamProperties);
    //~~~~~~~~~~~~~~~~~~~~ param creator for new instance~~~~~~~~~~~~~~~~~~~~
    void createParamsScalar(QMap<QString, ParamProperties>::const_iterator it);
    void createParamsEnumList(QMap<QString, ParamProperties>::const_iterator it);
    void createParamsChoiceList(QMap<QString, ParamProperties>::const_iterator it);
    void createItemParams(const QMap<QString, ParamProperties>& aParamProperties);
};
#ifdef false
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class  MultipleChoices : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString      key         READ key        CONSTANT)
    Q_PROPERTY(bool         optional    READ optional   CONSTANT)
    Q_PROPERTY(QString      description READ description CONSTANT)
    Q_PROPERTY(QStringList  choiceList  READ choiceList CONSTANT )

public:
    // Default constructor
    explicit MultipleChoices(QObject *parent = nullptr)
        : QObject(parent)       ,
          mKey                ()  ,
          mOptional           ()  ,
          mDescription        ()  ,
          mChoiceList         ()  ,
          mSelectedChoices    ()  ,
          mCheckedList        ()
    {
    }

    explicit MultipleChoices(const QString& aKey                        ,
                             const ParamProperties& aProps              ,
                             const QStringList & aChoiceList            ,
                             const QStringList & aSelectedChoices       ,
                             QObject *parent = nullptr)
        : QObject(parent),
          mKey                (aKey),
          mOptional           (aProps.mP_qual==Param_quality::cOptional),
          mDescription        (aProps.mDescription),
          mChoiceList         (aChoiceList),
          mSelectedChoices    (aSelectedChoices),
          mCheckedList        (  )  // reduced list
    {
        fromSelectedChoices(mChoiceList,mSelectedChoices);
    }

    // Destructor
    ~MultipleChoices() override {}

    // Copy constructor and Assignement operator
    MultipleChoices(const MultipleChoices &other):
        QObject(other.parent()) {*this=other;}

    MultipleChoices& operator=(const MultipleChoices &other)
    {return *other.clone(); }
    /// Clone pointed data (usefull for default value)
    MultipleChoices* clone() const {
        MultipleChoices* newValue = new MultipleChoices();
        newValue->mKey            =mKey            ;
        newValue->mOptional       =mOptional       ;
        newValue->mDescription    =mDescription    ;
        newValue->mChoiceList     =mChoiceList     ;
        newValue->mSelectedChoices=mSelectedChoices;
        newValue->mCheckedList    =mCheckedList    ;

        return newValue;
    }

    Q_INVOKABLE void selectedChoice(int aIndex, bool aStatus) { mCheckedList[aIndex]=aStatus; }
    Q_INVOKABLE bool isChoiceSelected(int aIndex) { return mCheckedList[aIndex]; }

    // Property accessors QML
    QString         key       (){return mKey       ;}
    bool            optional  (){return mOptional  ;}
    QString         description   (){return mDescription   ;}
    QStringList     choiceList(){return mChoiceList;}

    QVector<bool> fromSelectedChoices(const QStringList& aChoiceList, const QStringList& aSelectedChoices) {
        mCheckedList = QVector<bool>(aChoiceList.size(),false);

        for (int i = 0; i < aChoiceList.size(); i++) mCheckedList[i] = aSelectedChoices.contains(aChoiceList.at(i));

        return mCheckedList;
    }

    // accessors C++
    /*!
     * \brief setList (from possible entries and selected entries)
     * \param aChoiceList   : possible entries
     * \param aList         : selected entries
     */
    void   setList(QStringList aChoiceList,QStringList aList)
    {
        mChoiceList=aChoiceList;mCheckedList.fill(false,aChoiceList.size());
        for(int i=0;i<aList.size();i++)
        {
            int index=aChoiceList.indexOf(aList[i]);
            if(index>=0) mCheckedList[index]=true;
        }
        mSelectedChoices = aList;
    }

    QStringList&    getSelectedChoices()   { mSelectedChoices.clear();
                                              for(int i=0;i<mCheckedList.size();i++)
                                                  if(mCheckedList[i]) mSelectedChoices.append(mChoiceList[i]);
                                              return mSelectedChoices;}

public slots:

private :
    QString         mKey            ;
    bool            mOptional       ;
    QString         mDescription    ;
    QStringList     mChoiceList     ;
    QStringList     mSelectedChoices;
    QVector<bool>   mCheckedList    ;
};
#endif
Q_DECLARE_METATYPE(ParamValue)
Q_DECLARE_METATYPE(ParamList)

#endif // DATAITEM_H
