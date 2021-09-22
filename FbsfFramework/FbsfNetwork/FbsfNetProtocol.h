#ifndef FbsfNetProtocol_H
#define FbsfNetProtocol_H

#include <QtCore>
#include <QDebug>
#include <QVariant>
#include <QStringList>
#include <QMap>

#include "FbsfNetGlobal.h"

class  FBSF_INTERFACE FbsfNetProtocol
{
public:
        enum MessageType { CLIENT_INFO = 0,
                           MESSAGE,
                           MULTICAST,
                           EXECUTIVE,
                           DATA,
                           DATASET,
                           DATASET_VALUES,
                           NEW_CLIENT,
                           CLIENT_EXIT,
                           CLIENTS_LIST
                         };

        FbsfNetProtocol();
        explicit FbsfNetProtocol(const QByteArray &fullMessage);
        FbsfNetProtocol(const FbsfNetProtocol &p);
        ~FbsfNetProtocol();
        FbsfNetProtocol & operator = (const FbsfNetProtocol &p);
        friend QDebug operator << (QDebug d, const FbsfNetProtocol &p);

        MessageType             getHeader() const { return header; }
        void                    setHeader(const MessageType &type) { header = type; }
        QString                 getBody() const { return body; }
        void                    setBody(const QString &message) { body = message; }
        QMap<QString,QVariant>  Parameter() const { return parameterMap; }
        QVariant                Parameter(const QString &key) const { return parameterMap.value(key); }
        void                    Parameter(const QString &name, const QString &value) { parameterMap.insert(name,value); }
        void                    Parameter(const QString &name, const QVariant& value) { parameterMap.insert(name,value); }

        void                    Parameter(const QMap<QString,QVariant> &commands) { parameterMap = commands; }
        void                    convertFromStream(const QByteArray &fullMessage);
        QByteArray              convertToStream() const;

private:
        MessageType             header;
        QString                 body;
        QMap<QString,QVariant>  parameterMap;
};

#endif // FbsfNetProtocol_H
