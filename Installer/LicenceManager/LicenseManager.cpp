#include "LicenseManager.h"
#include <QFile>
#include <QNetworkConfigurationManager>

#define FILENAME "FbsfFramework.lic"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LicenseManager::LicenseManager()
    : settings(FILENAME, QSettings::IniFormat)
{
    QFile licenseFile;
    licenseFile.setFileName(FILENAME);
    licenseFile.open(QFile::WriteOnly | QFile::Truncate);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LicenseManager::LicenseManager(QString file)
    : settings(file, QSettings::IniFormat)
{
    // Check if the license file exist
    QFile licenseFile(file);
    if (!licenseFile.exists())
    {
        QString msg="License file "+ file + " not found";
        qWarning(msg.toStdString().c_str());
    }
    QString lic_hostid=settings.value("HOSTID").toString();
    if (lic_hostid=="ANY")
    {
        key_hostid=QByteArray("ANY");
    }
    else
    {
        // Get mac address for local host
        QString macaddress=getMacAddress(lic_hostid);
        key_hostid=macaddress.toUtf8();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool LicenseManager::check()
{
    QString lic_date=settings.value("DATE").toString();
    QString lic_appsign=settings.value("SIGN").toString();

    qDebug() << "License manager : ";
    // Check sign key
    QByteArray sign=gencode(lic_date);
    if (lic_appsign.toUtf8()!=sign)
    {
        qDebug() << "\tLicense mismatch, check your license file\n";
        //qDebug() << lic_appsign.toUtf8()<< "\n" << sign;
        if (key_hostid.isEmpty())
        {
            qDebug() << "\tthe expected license MAC Address"
                     << "doesn't match any network interfaces";
        }
        return false;
    }

    // Check the date key
    if (lic_date!="permanent")
    {
        QDate now = QDate::currentDate();
        int remain=now.daysTo(QDate::fromString(lic_date, "d-M-yyyy"));
        if (remain <= 0)
        {
            qDebug() << "\tApplication license period has expired";
            return false;
        }
        else
        {
            qDebug() << "\tApplication license period :"
                     << QDate::currentDate().toString( "d-M-yyyy")
                     << remain << "days remaining before expiration";
        }
    }
    else  qDebug() << "\tApplication license period : permanent";
    // store all features keys
    QStringList featuresList=settings.childGroups();
    for (int i=0 ; i < featuresList.length(); i++)
    {
        settings.beginGroup(featuresList[i]);
        QString date=settings.value("DATE").toString();
        QString sign=settings.value("SIGN").toString();
        feature* f=new feature(date,sign);
        features.insert(featuresList[i],f);
        settings.endGroup();
    }
    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool LicenseManager::check(QString aFeature)
{
    check();
    if (!features.contains(aFeature))
    {
        qDebug() << "\tFeature "<< aFeature << "unknown";
        return false;
    }
    feature* f=features.value(aFeature);

    if (f->sign.toUtf8()!=gencode(aFeature,f->date))
    {
        qDebug() << "\tFeature "<< aFeature<< "signature mismatch";
    }
    else
    {
        if (f->date!="permanent")
        {
            QDate now = QDate::currentDate();
            int remain=now.daysTo(QDate::fromString(f->date, "d-M-yyyy"));
            if (remain <= 0)
            {
                qDebug() << "\tFeature" << aFeature
                         << "license period has expired since"
                         << f->date;
                return false;
            }
            else
            {
                qDebug() << "\tFeature" << aFeature
                         << "license period :"<< remain
                         << "days remaining before expiration";
            }
        }
        else
            qDebug() <<  "\tFeature" << aFeature
                      << "license period : permanent";
    }
    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString LicenseManager::getMacAddress(QString lic_hostid)
{
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        // Return a matching MAC Address
        if (lic_hostid==interface.hardwareAddress())
            return interface.hardwareAddress();
    }
    return QString();
}
