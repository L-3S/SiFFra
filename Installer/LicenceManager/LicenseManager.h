#ifndef LICENSEMANAGER_H
#define LICENSEMANAGER_H
#include <QString>
#include <QMessageAuthenticationCode>
#include <QSettings>

#include <QtNetwork/QNetworkInterface>
#include <QDate>

#include <QDebug>

static QByteArray seed = "A8098FCB654323AD";

class LicenseManager
{
public:
    LicenseManager();
    explicit LicenseManager(QString file);

    QString getMacAddress(QString lic_hostid);

    bool check();
    bool check(QString aFeature);
    QByteArray gencode(QString date)
    {
        QMessageAuthenticationCode code(QCryptographicHash::Sha1);
        code.reset();
        code.setKey(key_hostid);
        code.addData(seed);
        code.addData(date.toStdString().c_str());
        //qDebug() << "gencode " <<  key_hostid << date << code.result().toHex() ;
        return code.result().toHex();
    }
    QByteArray gencode(QString feature,QString date)
    {
        QMessageAuthenticationCode code(QCryptographicHash::Sha1);
        code.reset();
        code.setKey(key_hostid);
        code.addData(seed);
        code.addData(QByteArray (feature.toStdString().c_str()));
        code.addData(date.toStdString().c_str());
        //qDebug() << "gencode " <<  key_hostid << feature <<  date << code.result().toHex() ;
        return code.result().toHex();
    }
    void    setHostid(QString hostid) {key_hostid=hostid.toStdString().c_str();}

    QSettings                   settings;


private :
    class feature
    {
    public:
        feature(QString d,QString s):date(d),sign(s){}
        QString date;
        QString sign;
    };


    QString                     macaddress;
    QByteArray                  key_hostid;
    QHash <QString, feature*>   features;
};

#endif // LICENSEMANAGER_H
