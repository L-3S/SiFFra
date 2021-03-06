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

QString ParamValue::dateFormat("");// default format
QString ParamValue::timeFormat("");// default format
//QString ParamValue::typeChoiceList("choiceList");
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// process the data properties with xml values
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::setParamsScalar(QMap<QString,QString>& aXmlParamList,
                                const QMap<QString, ParamProperties>::const_iterator it)
{
    // skip name as it is displayed as a main info
    if(it.key()=="name") return;

    QString key=realKey(it.key());

    if (it->mP_qual==Param_quality::cOptional) {
        ///~~~~~~~~~~~~~~~~~~~~ optional ~~~~~~~~~~~~~~~~~~~~~~~~
        if (aXmlParamList.contains(key)){
            /// add value from xml
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add optional value"
                     << key << it.value().mDescription << aXmlParamList.value(key);
#endif
            addData(key,aXmlParamList.value(key),it.value(),true);
        }
        else {
            /// add default value
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add optional default"
                     << key << it.value().mDescription <<it->mDefault;
#endif
            addData(key,it->mDefault,it.value());
        }
    }
    else
    {  ///~~~~~~~~~~~~~~~~~~~~ mandatory ~~~~~~~~~~~~~~~~~~~~~~~~
        if (aXmlParamList.contains(key))
        {
            /// add value from xml
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add mandatory value"
                     << key << it.value().mDescription << aXmlParamList.value(key);
#endif
            addData(key,aXmlParamList.value(key),it.value());
        }
        else
        {
            /// add an empty value
            addData(key,"",it.value());
#ifdef TRACE
            qDebug() << __FUNCTION__<< "add empty value"
                     << key << it.value().mDescription;
#endif
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::setParamsEnumList(QMap<QString,QString>& aXmlParamList,
                                  const QMap<QString, ParamProperties>::const_iterator it)
{
    QString key=realKey(it.key());

    if (it->mP_qual==Param_quality::cOptional) {
        ///~~~~~~~~~~~~~~~~~~~~ optional ~~~~~~~~~~~~~~~~~~~~~~~~
        if (aXmlParamList.contains(key) && !aXmlParamList.value(key).isEmpty())
        {
            /// add value from Xml
            addData(key,it->mEnumEntries,it.value(),aXmlParamList.value(key),true);
        }
        else {
            /// add default value
            addData(key,it->mEnumEntries,it.value(),it.value().mDefault);
        }
    }
    else {
        ///~~~~~~~~~~~~~~~~~~~~ mandatory ~~~~~~~~~~~~~~~~~~~~~~~~
        if (aXmlParamList.contains(key)){
            /// add value from Xml
            addData(key,it->mEnumEntries,it.value(),aXmlParamList.value(key));
        }
        else {
            /// add empty value
            addData(key,it->mEnumEntries,it.value(),QString(""));
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

    // If set we store the timeFormat once.
    if(aXmlParamList.contains("timeFormat"))
        ParamValue::timeFormat=aXmlParamList.value("timeFormat");

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

    QString key=realKey(it.key());

    if (it->mP_qual==Param_quality::cOptional)
    {
        addData(key,it->mDefault,it.value());
#ifdef TRACE
        qDebug() << __FUNCTION__<< "create optional with default"
                     << key << it.value().mDescription <<it->mDefault;
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
        addData(key,"",it.value());
#ifdef TRACE
        qDebug() << __FUNCTION__<< "create mandatory with empty value"
                     << key << it.value().mDescription;
#endif
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ParamList::createParamsEnumList(QMap<QString, ParamProperties>::const_iterator it)
{
    QString key=realKey(it.key());

    if (it->mP_qual==Param_quality::cOptional)
    {
        /// optional : add default value
        addData(key,it->mEnumEntries,it.value(),it->mDefault);
    }
    else
    {
        /// mandatory : add empty value
        addData(key,it->mEnumEntries,it.value(),QString(""));
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//void ParamList::createParamsChoiceList(QMap<QString, ParamProperties>::const_iterator it)
//{
//    addData(it.key(),it->mEnumEntries,it.value(),QString(""));
//}
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
