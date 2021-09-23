#ifndef PARAMPROPERTIES_H
#define PARAMPROPERTIES_H

#include <QString>
#include <QVariant>
<<<<<<< HEAD

=======
#include <QVector>
>>>>>>> 35df7cd96beac3585b743a8b67e2c465ba92bacd

enum class Param_quality: int
{
    cMandatory,
    cOptional
};


/// Dealing with the parameter type:
/// cStr:               string
/// cDbl:               double
/// cInt:               integer
/// cBool:              boolean
/// cPath:              path to a file
/// cDateAndTime:       Date and Time yyyy MM dd HH:mm:ss format (e.g. 1910 06 16 12:11:00)
/// cDateAndTimeUTC:    Date and Time (format UTC e.g. 2015-03-25T12:00:00Z)
/// cEnumString:        Enumerated list of String
/// cEnumInt:           Enumarated list of Int
enum class Param_type: int
{
    cStr,
    cDbl,
    cInt,
    cBool,
    cPath,
    cFilePath,
    cDateAndTime,
    cDateAndTimeUTC,
    cEnumString,
    cEnumInt,
    cChoiceList,
};

class ParamProperties
{
public :
    ParamProperties()= default;

    ParamProperties(const Param_quality aP_qual,
                    const Param_type aP_type,
                    const QString & aDescription,
                    const QString & aUnit,
                    const QVariant & aHint,
                    const QVector<QVariant> & aEnumEntries = QVector<QVariant>(),
                    const QVariant & aDefault = QVariant(),
                    const QVariant & aMin_strict = QVariant(),
                    const QVariant & aMax_strict = QVariant(),
                    const QVariant & aMin_warn = QVariant(),
                    const QVariant & aMax_warn = QVariant()
            ) :
        mP_qual(aP_qual),
        mP_type(aP_type),
        mDescription(aDescription),
        mUnit(aUnit),
        mHint(aHint),
        mEnumEntries(aEnumEntries),
        mDefault(aDefault),
        mMin_strict(aMin_strict),
        mMax_strict(aMax_strict),
        mMin_warn(aMin_warn),
        mMax_warn(aMax_warn)
    {
    }

    Param_quality   mP_qual;
    Param_type      mP_type;

    /// A comment to describe the field
    QString mDescription;

    /// A unit for numeric types
    QString mUnit;

    /// Hint value (to guide user)
    QVariant        mHint;

    /// List of possible entries for enumarated types
    QVector<QVariant> mEnumEntries;

    /// Default value in case of optionnal parameter
    QVariant        mDefault;

    /// Bounding variants for ordered types
    QVariant mMin_strict, mMax_strict;
    QVariant mMin_warn, mMax_warn;


};

#endif // PARAMPROPERTIES_H
