#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QDateTime>
#include <iostream>
#include "FbsfReplayFileAPI.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc < 2 )
    {
        qFatal(QString( "usage : %1 <data file>")
               .arg( argv[0] ).toStdString().c_str());
    }
    // create the api
    FbsfReplayFileAPI  api(argv[1]);
    // get the number of computed steps
    int nbSteps=api.getReplayDuration();
    // iterate trough data list and get data informations
    QList<QString> dataList;
    api.getDataList(dataList);
    QListIterator<QString> iter(dataList);
    while(iter.hasNext())
    {
        DataInfo data(iter.next());
        qDebug() << data.Name() << " : "
                 << data.Producer()
                 << data.ClassString()
                 << data.TypeString()
                 << data.Description()
                 << data.Unit()
                 << data.TimeShift()
                 << data.TimeIndex();
    }
    if(api.isStandardMode())
    {
        //~~~~~~~~~~~~~ get some data values for standard mode ~~~~~~~~~~~~~~
        DataInfo dataTime("Simulation.Time");
        DataInfo data1("Logic.Constant#1.k");


        qDebug() << data1.Name()
                 << (data1.Class()==DataInfo::cScalar? "is Scalar":"is Vector")
                 << (data1.Type()==DataInfo::cInteger? "Integer":"Real");

        // iterate over the steps to produce a csv format
        for (int i = 0 ; i < nbSteps; i++)
        {
            if (i==0)
            {
                // the title row
                qDebug() << dataTime.Name() << ";" << data1.Name() ;

            }
            // the value rows
            qDebug() << dataTime.Values()[i].toFloat()  << data1.Values()[i].toInt() ;
        }
    }
    else
    {
        //~~~~~~~~~~~~~ get data values for Time Depend mode ~~~~~~~~~~~~~~
        qDebug() << "data file :" << argv[1] << "\nData time depend mode";

        // Case TD vector integer and TD vector real
        DataInfo data1("Data.Time");
        DataInfo data2("SignalAnalyzer.SawTooth#1.amplitude");

        for (int i = nbSteps-1 ; i < nbSteps; i++) // last step
        {
            std::cout  << std::endl
                       << i << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
                       << std::endl;
            QVariantList timeValues = data1.TimeDependValues(i);
            QVariantList data2Values = data2.TimeDependValues(i);

            for (int j = 0 ; j < timeValues.length(); j++)
            {
                QDateTime date = QDateTime::fromSecsSinceEpoch(timeValues[j].toInt());
                QString dateStr= date.toString(QString("d MMM yyyy h:m:s")) ;
                std::cout << dateStr.toUtf8().constData()<< " ";
            }
            std::cout  << std::endl;

            for (int j = 0 ; j < data2Values.length(); j++)
                if(data2.Type()==DataInfo::cInteger)
                    std::cout << data2Values[j].toInt() << " ";
                else
                    std::cout << data2Values[j].toReal() << " ";
            std::cout  << std::endl;

//            for (int j = 0 ; j < data3Values.length(); j++)
//                if(data2.Type()==DataInfo::cInteger)
//                    std::cout << data3Values[j].toInt() << " ";
//                else
//                    std::cout << data3Values[j].toReal() << " ";
            std::cout  << std::endl;
        }
    }
}
