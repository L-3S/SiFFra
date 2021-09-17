import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSISO
{
    nodeType: "Derivative"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set state variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // uOld: state value for u
    // initialise and defined during mFirstStep

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yStart : start value
    property var parameters : {"yStart" :0 }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
        //console.log("Initialisation de ", nodeName, " valeur du timestep ", timeStep);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
             statesvar.mFirstStep = false;
            // yStart = (input.value-statesvar.uOld)/timeStep
            // Firts value of uOld is computed to reach yStart at Init
            statesvar.uOld = input.value - parameters.yStart*timeStep;
         }

        output.value = (input.value-statesvar.uOld)/timeStep;
        statesvar.uOld = input.value;
    }


}

