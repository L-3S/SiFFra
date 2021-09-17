import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0
Node
{
    nodeType    : "RealInputTV"
    shape.width : 100
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set output socket
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    socketsRight: [Socket {id: out;socketId:"Out";direction: "output";value:1}]
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subscription to a time depend vector
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //SubscribeVectorReal {id : timeVector; tag1: "Producer."+nodeName}
    SubscribeVectorReal {id : timeVector; tag1: nodeName}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
    property int timeshift
	property int timesindex
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Life start
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted :
    {
        // get timeshift (number of shifted steps)
        timeshift=timeVector.timeshift
        // get time index (current time data)
        timesindex=timeVector.timeindex
    }
    */
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var statesvar : {"mFirstStep": true,
                              "mlocSimuTime" : 0}

    property real tempsDeb;
    property int index;


    function compute()
    {
        if (statesvar.mFirstStep){
             statesvar.mFirstStep = false;
         }

        // Le simulationTime de FBSF s'incrémente de timeStep à chaque "cycle de calcul"
        // il devrait s'incrémenter de timeStep * timeshift
        // tempsDeb: intant physique correspondant au début du vecteur
        // cette grandeur est calculée à partir du simulationTime modifiée
        // L'index est calculé en fonction de la différence entre statesvar.mlocSimuTime et tempsDeb

        //console.log(nodeName+" object: temps simulé FBSF et temps simulé local", simulationTime - timeStep, statesvar.mlocSimuTime);

        tempsDeb = (simulationTime - timeStep)/timeStep * timeVector.timeshift * timeStep;
        index = ((statesvar.mlocSimuTime - tempsDeb) / timeStep) % timeVector.data.length;

        out.value = timeVector.data[index]
        //index = ++index % timeVector.data.length
        statesvar.mlocSimuTime += timeStep;
    }
}
