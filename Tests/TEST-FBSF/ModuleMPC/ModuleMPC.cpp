#include "ModuleMPC.h"

#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QRandomGenerator>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModuleMPC::ModuleMPC()
    : FBSFBaseModel()
    , mTimeStep(900)
    , unresolvedInt(0)
    , unresolvedFloat(0)
    , counter1(0)
    , counter2(0)
    , vectornumber(2)
    , pastsize(0)
    , futursize(0)
    , timeshift(0)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMPC::doInit()
{
    pastsize=AppConfig()["pastsize"].toInt();
    futursize=AppConfig()["futursize"].toInt();
    timeshift=AppConfig()["timeshift"].toInt();

    vectornumber=config()["vectornumber"].toInt();

    if (name() == "Producer")
    {
        // size and publish 2 real vector to QML
        ContainerVectorRealOut.resize(vectornumber);
        for (int i=0;i< vectornumber;i++)
        {
            ContainerVectorRealOut[i].resize(pastsize+futursize);
            publish(QString("VectorReal_%1").arg(i),
                    &ContainerVectorRealOut[i],"n/a",
                    QString("Output Real Vector from %1").arg(name()),
                    timeshift,pastsize);
        }
        // size and publish an int vector
        VectorIntOut.resize(pastsize+futursize);
        for (int i=0;i< 1;i++)
        {
            publish(QString("%1.VectorInt_%2").arg(name()).arg(i),
                    &VectorIntOut,"n/a",
                    QString("Output Int Vector from %1").arg(name()),
                    timeshift,pastsize);
        }
        // publish an int scalar
        publish(QString("%1.Counter").arg(name()),
                &counter1,"",
                QString("Output counter1 from %1").arg(name()));
        // publish a float scalar
        publish(QString("%1.Counter2").arg(name()),
                &counter2,"",
                QString("Output counter2 from %1").arg(name()));
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (name() == "Consumer")
    {
        // subscribe to several real vector
        for (int i=0;i< vectornumber;i++)
            subscribe(QString("VectorReal_%1").arg(i),
                      &VectorRealIn,"n/a",
                      "Input Real Vector in Consumer");


        // subscribe to one int vector
        subscribe("Producer.VectorInt_0",
                  &VectorIntIn,"n/a",
                  "Input Int Vector from Producer");
        // subscribe to int scalar
        subscribe("Producer.Counter",&counter1,"","consume counter from Producer");
        subscribe("Producer.Counter2",&counter2,"","consume counter2 from Producer");
        // subscribe to unresolved
        subscribe("Unresolved Float",&unresolvedFloat,"xxxxxx","free value");
        subscribe("Unresolved Int",&unresolvedInt,"xxxxxx","free value");
    }
    return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <QThread>
int ModuleMPC::doStep()
{//QThread::msleep(20);

    for (int iStep=0;iStep<timeshift;iStep++)
    {
        counter1+=1 ;
        counter2 +=0.1f;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if (name() == "Producer")
        {
            // update vector of integer
            for (int i=0;i < VectorIntOut.size();i++)
            {
                //VectorIntOut[i]=i%2==0?2:counter1%10;
                for (int i=0;i < pastsize;i++)
                    VectorIntOut[i]=counter1+i;//QRandomGenerator::global()->generate()%100+1;
                for (int i=pastsize;i < VectorIntOut.size();i++)
                    VectorIntOut[i]=counter1;
            }
            // update vectors of real
            for (int i=0;i< vectornumber;i++)
            {
                for (int j=0;j < pastsize;j++)
                    ContainerVectorRealOut[i][j]=counter2;//QRandomGenerator::global()->generate()%100+1;
                for (int j=pastsize;j < ContainerVectorRealOut[i].size();j++)
                    ContainerVectorRealOut[i][j]=j%2==0?2.0:counter2;
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    return 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMPC::doTerminate()
{
    return 1;
}
