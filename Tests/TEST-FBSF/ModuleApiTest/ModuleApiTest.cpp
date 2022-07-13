#include "ModuleApiTest.h"

#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QRandomGenerator>
#include <chrono>
#include <thread>

using namespace std::chrono;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModuleApiTest::ModuleApiTest()
    : FBSFBaseModel()
    , unresolvedInt(0)
    , unresolvedFloat(0)
    , counter1(0)
    , random_pos(0)
    , random_neg(0)
    , VectorInt(10)
    , VectorReal(10)
    , vectornumber(1)
{
    VectorInt = QVector<int>(10);
    for (int o = 0; o < VectorInt.size(); o++) {
        VectorInt[o] = 0;
    }
    VectorIntPt = new QVector<int>(10);
    for (int o = 0; o < VectorIntPt->size(); o++) {
        (*VectorIntPt)[o] = 0;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleApiTest::doInit()
{
    vectornumber=config()["vectornumber"].toInt();

    if (name() == "Producer")
    {
        // subscribe to a vector of integer exported from QML
        for (int i = 0 ; i < UIVectorIntIn.size();i++ )UIVectorIntIn[i]=0;
        subscribe("UIManual.VectorInt",&UIVectorIntIn);
        // subscribe to a vector of real exported from QML
        for (int i = 0 ; i < UIVectorRealIn.size();i++ )UIVectorRealIn[i]=0;
        subscribe("UIManual.VectorReal",&UIVectorRealIn);
        // publish an int scalar
        publish(QString("%1.Counter").arg(name()),
                &counter1,"J",
                QString("Output counter value from %1").arg(name()));
        // Publish  2 real scalar
        publish(QString("%1.random_pos").arg(name()),
                &random_pos,"K",
                QString("Output positive random from %1").arg(name()));

        publish(QString("%1.random_neg").arg(name()),
                &random_neg,"K",
                QString("Output negative random from %1").arg(name()));
        // publish a real scalar to modify some parameter
        publish(QString("Logic-out.Constant#1.k"),
                &param1,"n/a",
                QString("Output parameter from %1").arg(name()));

        // publish a int vector
        publish(QString("%1.VectorInt").arg(name()),
                &VectorInt,"Km/h",
                QString("Output VectorInt from %1").arg(name()));
        // Publish a real vector
        publish(QString("%1.VectorReal").arg(name()),
                &VectorReal,"m3/s",
                QString("Output VectorReal from %1").arg(name()));
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    else if (name() == "Consumer")
    {
        // subscribe to int scalar
        subscribe("Producer.Counter",&counter1,"","consume counter1 from Producer");
        subscribe("Producer.random_pos",&random_pos,"","consume random from Producer");
        subscribe("Producer.random_neg",&random_neg,"","consume random from Producer");
        // subscribe to unresolved
        subscribe("Unresolved Float",&unresolvedFloat,"xxxxxx","free value");
        subscribe("Unresolved Int",&unresolvedInt,"xxxxxx","free value");
    }
    if (name() == "Producer" || name() == "ModuleBatch")
    {
        // Publish a real value to FMU input
        publish(QString("TuningFMU.titi"),
                &param1,"n/a",
                QString("Output to FMU from %1").arg(name()));
        // Get the output value from fmu
        subscribe("TuningFMU.OutPutFMU",&FmuOutput,"","Output from FMU");
    }

    // publish a int vector
    publish(QString("ModuleBatch.VectorInt").arg(name()),
            &VectorInt,"Km/h",
            QString("Output VectorInt from %1").arg(name()));
    publish(QString("%1.VectorIntPT").arg(name()),
            VectorIntPt,"Km/h",
            QString("Output VectorInt from %1").arg(name()));
    publish(QString("Pump.Speed"),
            &pa,"n/a",
            QString("Output to FMU from %1").arg(name()));
    mStateDataValueMap["app"] = 110;
    publish(QString("resetTet"),
            (int*)mStateDataValueMap["app"].data(),"n/a",
            QString("Output to FMU from %1").arg(name()));
    return FBSF_OK;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleApiTest::doStep(int timeOut)
{
    clock_t start, end;
    start = clock();
//    QTimer mt;
//    std::cout << "Antoine timout" << timeOut<< std::endl;
//    t.start(timeOut);
//    mt.start(2000);
//    while (1 && (t.remainingTime() >= 1) && (timeOut != -1)) {
//        std::cout << "Antoine c timout" << timeOut << std::endl;
//    }
//    if (t.remainingTime()<= 1 && mt.remainingTime() >= 1) {
//        return FBSF_TIMEOUT;
//    }
//    std::cout << "Antoine cend timout" << timeOut<< std::endl;
    pa = 15;
    *((int*)mStateDataValueMap["app"].data()) += 1;
    if (name() == "Producer" || name() == "ModuleBatch")
    {
        param1  = counter1%10;
        counter1 +=1 ;
        qDebug() << "Antoine in dostep" << VectorInt  << &VectorInt<< VectorIntPt;
        for (int o = 0; o < VectorInt.size(); o++) {
            VectorInt[o] = VectorInt[o] + 1;
        }
        for (int o = 0; o < VectorIntPt->size(); o++) {
            (*VectorIntPt)[o] += o;
        }
        qDebug() << "Antoine aftin dostep" << VectorInt  << &VectorInt<< VectorIntPt;

        random_pos =250+QRandomGenerator::global()->generate()%100;
        random_neg =250-QRandomGenerator::global()->generate()%100;
        for (int i = 0 ; i < UIVectorIntIn.size();i++ )VectorInt[i]= UIVectorIntIn[i];
        for (int i = 5 ; i < VectorInt.length();i++ )VectorInt[i]= QRandomGenerator::global()->generate()%100+1;
        for (int i = 0 ; i < UIVectorRealIn.size();i++ )VectorReal[i]= UIVectorRealIn[i];
        for (int i = 5 ; i < VectorReal.length();i++ )VectorReal[i]= QRandomGenerator::global()->generate()%100+1;
        // Perfmeter test
        for(int i=0;i<param1*100000;i++){QRandomGenerator::global()->generate();} // load CPU

        //if (mSimulationTime>10) return FBSF_ERROR;
        if (name() == "ModuleBatch")
        {
            if(counter1>=20)emit ExecutiveControl("stop");// test BATCH mode
            qDebug() << __FUNCTION__
                     << "param1 :" << param1
                     << "FmuOutput :" << FmuOutput;
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    end = clock();
    return ((((float) end - start)/CLOCKS_PER_SEC) > (((float)timeOut) / 1000) ? FBSF_TIMEOUT : FBSF_OK);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleApiTest::doTerminate()
{
    return FBSF_OK;
}