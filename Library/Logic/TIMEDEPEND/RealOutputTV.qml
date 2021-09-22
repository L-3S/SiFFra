import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
Node
{
    nodeType    : "RealOutputTV"
    shape.width : 100
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set output socket
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    socketsLeft: [Socket {id: input;socketId:"In";direction: "input"}]

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Publish a time depend vector
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SignalVectorReal {
        id      : timeVector
        tag1    : nodeName
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Life start
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted :
    {
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var statesvar : {"mFirstStep": true,
                              "mlocSimuTime" : 0}


    property real tempsDeb;
    property int index;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous initialization
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    function initialize()
    {
        timeVector.timeindex=pastSize
        timeVector.timeshift=timeShift
        timeVector.data=new Array(pastSize+futurSize)

        // Initialisation à NaN, notamment pour le NoFutureProjection
        for (var i = 0; i < timeVector.data.length; i++)
            timeVector.data[i] = NaN
    }
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

        timeVector.data[index]=input.value
        statesvar.mlocSimuTime += timeStep;
    }
}
