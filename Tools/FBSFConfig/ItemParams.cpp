#include "ItemParams.h"
#include "ItemProperties.h"

//#define TRACE
QString ParamValue::typeInt("int");
QString ParamValue::typeNumber("number");
QString ParamValue::typeString("string");
QString ParamValue::typeBool("bool");
QString ParamValue::typeCheckable("checkable");
QString ParamValue::typeStringList("list");
QString ParamValue::typeIntList("list");// same as string list
QString ParamValue::typePath("path");
QString ParamValue::typeFilePath("filepath");
QString ParamValue::typeDateAndTime("DateAndTime");
QString ParamValue::typeDateAndTimeUTC("DateAndTimeUTC");
QString ParamValue::typeDate("date");
QString ParamValue::typeTime("time");

QString ParamValue::dateFormat(defaultDateFormat);// default format
//QString ParamValue::typeChoiceList("choiceList");
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// process the data properties with xml values
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::setParamsScalar(QMap<QString,QString>& aXmlParamList,
                                const QMap<QString, ParamProperties>::const_iterator it)
{
    // skip name as it is displayed as a main info
    if(it.key()=="name") return;

    if (it->mP_qual==Param_quality::cOptional) {
        if (aXmlParamList.contains(it.key())){
            /// add value from xml
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add optional value"
                     << it.key() << it.value().mDescription << aXmlParamList[it.key()];
#endif
            addData(it.key(),aXmlParamList[it.key()],it.value());
        }
        else {
            /// add default value
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add optional default"
                     << it.key() << it.value().mDescription <<it->mDefault;
#endif
            addData(it.key(),it->mDefault,it.value());
        }
    }
    else
    {  /// mandatory
        if (aXmlParamList.contains(it.key()))
        {
            /// add value from xml
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add mandatory value"
                     << it.key() << it.value().mDescription << aXmlParamList[it.key()];
#endif
            addData(it.key(),aXmlParamList[it.key()],it.value());
        }
        else
        {
            /// add an empty value
            addData(it.key(),"",it.value());
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add empty value"
                     << it.key() << it.value().mDescription;
#endif
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::setParamsEnumList(QMap<QString,QString>& aXmlParamList,
                                  const QMap<QString, ParamProperties>::const_iterator it)
{
    if (it->mP_qual==Param_quality::cOptional) {
        if (aXmlParamList.contains(it.key()) && !aXmlParamList[it.key()].isEmpty())
        {
            /// add value from Xml
            addData(it.key(),it->mEnumEntries,it.value(),aXmlParamList[it.key()]);
        }
        else {
            /// add default value
            addData(it.key(),it->mEnumEntries,it.value(),it.value().mDefault);
        }
    }
    else {
        /// mandatory
        if (aXmlParamList.contains(it.key())){
            /// add value from Xml
            addData(it.key(),it->mEnumEntries,it.value(),aXmlParamList[it.key()]);
        }
        else {
            /// add empty value
            addData(it.key(),it->mEnumEntries,it.value(),QString(""));
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::setParamsChoiceList(QMap<QString,QString>& aXmlParamList,
                                  const QMap<QString, ParamProperties>::const_iterator it)
{
    if (it->mP_qual==Param_quality::cOptional)
    {
        if (aXmlParamList.contains(it.key()) && !aXmlParamList[it.key()].isEmpty())
        {
            /// add value from Xml
            addData(it.key(),it->mEnumEntries,it.value(),aXmlParamList[it.key()]);
        }
        else {
            /// add default value
            addData(it.key(),it->mEnumEntries,it.value(),it.value().mDefault);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compare param property against xml param and set item param
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::setItemParams(QMap<QString,QString>& aXmlParamList,
                              const QMap<QString, ParamProperties>& aParamProperties)
{
    // If set we store the dateFormat once.
    if(aXmlParamList.contains("dateFormat"))
        ParamValue::dateFormat=aXmlParamList.value("dateFormat");

    QMap<QString, ParamProperties>::const_iterator it = aParamProperties.constBegin();
    while (it != aParamProperties.constEnd())
    {
        if (it->mP_type==Param_type::cEnumString || it->mP_type==Param_type::cEnumInt)
            setParamsEnumList(aXmlParamList,it);
        else
            setParamsScalar(aXmlParamList,it);
        ++it;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Item creation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::createParamsScalar(QMap<QString, ParamProperties>::const_iterator it)
{
    if(it.key()=="name") return; // skip name as it is displayed as main info
    if (it->mP_qual==Param_quality::cOptional)
    {
        addData(it.key(),it->mDefault,it.value());
#ifdef TRACE
        qDebug() << __FUNCTION__<< "create optional with default"
                     << it.key() << it.value().mDescription <<it->mDefault;
#endif
    }
    else
    {
        /// mandatory field
        /// the bahavior is not specified here
        /// possible choices are
        /// - add here an empty field to force the user to overwrite with a valid choice
        /// - add here the first or last element of the list
        /// we choose the conservative option
        addData(it.key(),"",it.value());
#ifdef TRACE
        qDebug() << __FUNCTION__<< "create mandatory with empty value"
                     << it.key() << it.value().mDescription;
#endif
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::createParamsEnumList(QMap<QString, ParamProperties>::const_iterator it)
{
    if (it->mP_qual==Param_quality::cOptional)
    {
        /// optional : add default value
        addData(it.key(),it->mEnumEntries,it.value(),it->mDefault);
    }
    else
    {
        /// mandatory : add empty value
        addData(it.key(),it->mEnumEntries,it.value(),QString(""));
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::createParamsChoiceList(QMap<QString, ParamProperties>::const_iterator it)
{
    addData(it.key(),it->mEnumEntries,it.value(),QString(""));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Compare param property against xml param and set item param
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::createItemParams(const QMap<QString, ParamProperties>& aParamProperties)
{
    QMap<QString, ParamProperties>::const_iterator it = aParamProperties.constBegin();
    while (it != aParamProperties.constEnd())
    {
        if (it->mP_type==Param_type::cEnumString || it->mP_type==Param_type::cEnumInt)
            createParamsEnumList(it);
//        else if (it->mP_type==Param_type::cChoiceList)
//            createParamsChoiceList(it);
        else
            createParamsScalar(it);
        ++it;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// check item param property
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool ParamValue::isInvalid()
{
    mHasError=false;
    if(mandatory() && getValue()=="")
    {
        error("empty value for " + key());
        mHasError=true;
    }
    // Check if value is in list
    else if(type()==ParamValue::typeStringList)
    {
        if(!value().toList().contains(getValue()))
        {
            error("Value not in list for " + key());
            mHasError=true;
        }
    }
    else if(type()==ParamValue::typeDate && !value().toString().isEmpty())
    {
        QString format=unit().isEmpty()?defaultDateFormat:unit();
        QDate isoDate=QDate::fromString(value().toString(),format);
        if(!isoDate.isValid())
        {
            error("not a valid date/format for " + key()
                         +", check format : "+value().toString()+"/"+unit());
            mHasError=true;
        }
    }
    else
    {
        // Check range for value
        switch (defaultType())
        {
        case QVariant::Bool   :
            if(value().toString().toLower()!="true"
                    &&value().toString().toLower()!="false")
            {
                error("not a boolean value for " + key());
                mHasError=true;
            }
            break;
        case QVariant::Int    :
            if (value().toInt()<minStrict().toInt()
                    || value().toInt()>maxStrict().toInt())
            {
                error("value out of range for " + key());
                mHasError=true;
            }
            break;
        case QVariant::Double :
            if (value().toDouble()<minStrict().toDouble()
                    || value().toDouble()>maxStrict().toDouble())
            {
                error("value out of range for " + key());
                mHasError=true;
            }
            break;
        case QVariant::LongLong :
            if (value().toLongLong()<minStrict().toLongLong()
                    || value().toLongLong()>maxStrict().toLongLong())
            {
                error("value out of range for " + key());
                mHasError=true;
            }
            break;
        default : break;
        }
    }
    return mHasError;
}
