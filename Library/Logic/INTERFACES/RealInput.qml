import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

PartialSO
{
    nodeType: "RealInput"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subscription to a external value
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Values for Cosimulation mode
    property real mScaValue: 0
    SubscribeReal{
        tag1: (!simuMpc) ? nodeName : "" ;
        onValueChanged: {mScaValue = value;}
    }

    /// Values for SimuMPC mode
    SubscribeVectorReal{
        id : timeVector;
        tag1: (simuMpc) ? nodeName : "" ;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
    }

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


        if (!simuMpc) {
            /// Cosimulation mode
            output.value = mScaValue
        }
        else {
            /// SimuMPC

            // Le simulationTime de FBSF s'incrémente de timeStep à chaque "cycle de calcul"
            // il devrait s'incrémenter de timeStep * timeshift
            // tempsDeb: intant physique correspondant au début du vecteur
            // cette grandeur est calculée à partir du simulationTime modifiée
            // L'index est calculé en fonction de la différence entre statesvar.mlocSimuTime et tempsDeb

            //console.log(nodeName+" object: temps simulé FBSF et temps simulé local", simulationTime - timeStep, statesvar.mlocSimuTime);

            tempsDeb = (simulationTime - timeStep)/timeStep * timeVector.timeshift * timeStep;
            index = ((statesvar.mlocSimuTime - tempsDeb) / timeStep) % timeVector.data.length;

            output.value = timeVector.data[index]
            //index = ++index % timeVector.data.length
            statesvar.mlocSimuTime += timeStep;
        }

    }
}

