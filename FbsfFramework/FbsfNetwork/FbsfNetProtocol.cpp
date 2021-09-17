#include "FbsfNetProtocol.h"
#include "FbsfNetGlobal.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*! \class FbsfNetProtocol
    \brief Network protocol for client and server messages
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetProtocol::FbsfNetProtocol()
{
    header = MESSAGE;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetProtocol::FbsfNetProtocol(const QByteArray &fullMessage)
{
    convertFromStream(fullMessage);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetProtocol::FbsfNetProtocol(const FbsfNetProtocol &p)
{
    header = p.header;
    body = p.body;
    parameterMap = p.parameterMap;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetProtocol::~FbsfNetProtocol()
{
    header = MESSAGE;
    body = "";
    parameterMap.clear();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfNetProtocol & FbsfNetProtocol::operator = (const FbsfNetProtocol &p)
{
    if (this != &p)
    {
        header = p.header;
        body = p.body;
        parameterMap = p.parameterMap;
    }

    return *this;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// For debugging purpose
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QDebug operator << (QDebug d, const FbsfNetProtocol &p)
{
    QDataStream msg(p.convertToStream());
    d << msg;

    return d;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Decoding message procedure
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfNetProtocol::convertFromStream(const QByteArray &fullMessage)
{
    QDataStream msg(fullMessage);
    int type;
    msg >> type;
    header = MessageType(type);
    msg >> body;
    int ParamNumber;
    msg >> ParamNumber;
    for (int i = 0; i < ParamNumber; i++)
    {
        QString ParamName;
        msg >> ParamName;
        QVariant ParamValue;
        msg >> ParamValue;
        parameterMap.insert(ParamName,ParamValue);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// Encoding message procedure
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QByteArray FbsfNetProtocol::convertToStream() const
{
    QByteArray block;
    QDataStream msg(&block, QIODevice::WriteOnly);
    msg << (quint32)0;

    int type = header;
    msg << type << body;

    QStringList keys = parameterMap.keys();
    msg << keys.size();
    foreach (QString key, keys)
    {
        msg << key << parameterMap.value(key);
    }

    msg.device()->seek(0);
    msg << (quint32)(block.size() - sizeof(quint32));
    return block;
}
