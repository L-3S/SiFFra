import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialDISO {
    nodeType: "VariableDelay"
    source  : nodeType+".svg"

    property int bufferSize : 10000;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set state variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // uHistory: buffer memory for input value
    // initialise and defined during mFirstStep


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize uHistory
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initHistory()
    {
        while (statesvar.uHistory.length <   bufferSize)
        {(statesvar.uHistory).push(input1.value)}

       // console.log("quelle est la longueur de  "+uHistory.length);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
                    (statesvar.uHistory) = [];
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
       if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;

           initHistory()
        }

       // ajoute un élément au début du tableau
       (statesvar.uHistory).unshift(input1.value);
       // supprime le dernier élément du tableau
       (statesvar.uHistory).pop();

       var index = 2;
       var boundedDelay = input2.value;

       if (boundedDelay <= 0) boundedDelay = 0;
       if (boundedDelay > (bufferSize - 1)*timeStep){
           console.exception(nodeName+" object: delay is too high and exceeds actual buffer size");
           console.log(nodeName+" object: delay from input2 is automatically decreased to (bufferSize - 1)*timeStep = "+(bufferSize - 1)*timeStep);
           boundedDelay = (bufferSize - 1)*timeStep;
       }

       index = Math.round(boundedDelay / (timeStep));

       if (index > bufferSize - 1) index = bufferSize - 1;

        output.value = (statesvar.uHistory)[index];
    }



}
