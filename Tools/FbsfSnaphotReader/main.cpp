#include <QCoreApplication>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include "FbsfGlobal.h"
#define PRINT qDebug().noquote() << fixed << qSetRealNumberPrecision(2)

void printMap(QVariantMap& aMap, QString category=QString())
{
    for (QVariantMap::iterator i = aMap.begin();
            i!= aMap.end();++i)
    {
        QVariant& data=aMap[i.key()];

        QString typeName=data.typeName();
        QMetaType::Type type=static_cast<QMetaType::Type>(data.type());

        if (typeName == "QVector<float>")
        {
            PRINT << category << typeName
                     << i.key() << " : " << data.value<QVector<float>>();
        }
        else if (typeName == "QVector<int>")
        {
            PRINT << category << typeName
                     << i.key() << " : " << data.value<QVector<int>>();
        }
        else if (type == QMetaType::QVariantMap)
        {   // Map data (Macro object)
            QVariantMap map=data.toMap();

            if(map.size()>0)
            {
                if(i.key() == "delayed")
                    printMap(map,"delayed");
                else if(i.key() == "parameters")
                    printMap(map,"parameter");
                else
                    printMap(map,category);
            }
        }
        else if (type == QMetaType::Bool)
        {
            PRINT << category << typeName << " "
                 << i.key() << " : " << data.toBool();
        }
        else if (type == QMetaType::Int)
        {
            PRINT << category << typeName << "  "
                 << i.key() << " : " << data.toInt();
        }
        else if (type == QMetaType::Float)
        {
            PRINT << category << typeName << " "
                 << i.key() << " : " << data.toFloat();
        }
        else if (type == QMetaType::Double)
        {
            PRINT << category << typeName
                 << i.key() << " : " << data.toDouble();
        }
        else if (type == QMetaType::QVariantList)
        {
            QVariantList v=data.toList();
            QMetaType::Type listType=static_cast<QMetaType::Type>(v[0].type());
            if (listType != QMetaType::QVariantList)
            {   // 1D array
                PRINT << category << "1D array" << i.key() << ":" << data.toStringList();
            }
            else
            {
                // 2D array
                PRINT << "2D array" << i.key() << " : ";
                QSequentialIterable iterable = data.value<QSequentialIterable>();
                foreach (const QVariant &v, iterable)
                {
                    PRINT << category << "\t" << v.toStringList();
                }
            }
        }
        else if (type == QMetaType::QByteArray)
        {
            PRINT << category << typeName
                 << i.key() << " : raw data" ;
        }
        else
        {
            PRINT << "unmanaged type :" << data.typeName();
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile snapFile(argv[1]);

    if (!snapFile.open(QIODevice::ReadOnly))
    {
        PRINT << "Could not read snapshot file:" << argv[1]
                 << "Error string:" << snapFile.errorString();
        return 0;
    }

    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaTypeStreamOperators<QVector<int>>("QVector<int>");

    qRegisterMetaType<QVector<float>>("QVector<float>");
    qRegisterMetaTypeStreamOperators<QVector<float>>("QVector<float>");
    QDataStream in(&snapFile);
    in.setVersion(QDataStream::Qt_5_7);

    // Read and check the file header
    quint32 magic;
    in >> magic;
    if (magic != FBSF_MAGIC)
    {
        QString msg("Snapshot file " + snapFile.fileName() +" : " +
                    "bad file format");
        QMessageBox::warning( 0, "[Error]", msg.toStdString().c_str());
//            qWarning(msg.toStdString().c_str());
    }
    // Get the format version
    quint32 format;
    in >>   format;
    // Get the framework version
    QString version;
    in >>   version;
    // Get the global simulation time
    float simulationTime;
    in >> simulationTime;

    // For each modules restore the states data
    PRINT << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
             << "FBSF snapshot version : " + version
             << "Simulation time : " << simulationTime/1000 << "ms"
             << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    while (!in.atEnd())
    {
        QString vName; // TODO test from executive
        in >> vName;
        in >> simulationTime;
        PRINT << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
                 << "\nModule : " + vName
                 << "\nSimulation time : " << simulationTime << "s"
                 << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
        QVariantMap map;
        in >> map;

        printMap(map,"state  ");
    }

    snapFile.close();
}
