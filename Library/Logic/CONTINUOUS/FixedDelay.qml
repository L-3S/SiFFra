import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSISO {
    nodeType: "FixedDelay"
    source  : nodeType+".svg"

    //property int bufferSize : 20;
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
    // maxDelay: maxDelayTime
    property var parameters : {
        "bufferSize" : 20,
        "delay" : 18
    }




    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check parameters consistency
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function checkParaConsistency()
    {

        /// WARNING: bufferSize should not be changed during runtime but there is no mean to test it here
        /// !!!!!!

        // TESTING AND COMPUTING TO DO ONLY AT INIT OR
        // AFTER PARAMETER CHANGE BY USER IN EDITOR
        // Consistence des paramètres d'entrée
        parameters.bufferSize = Math.floor(parameters.bufferSize); /// To round (parameter should be an integer)

        if (parameters.bufferSize < 1)
        {
            console.exception(nodeName+" object: please revise parameters to ensure bufferSize >= 1");
            console.log(nodeName+" object: parameter bufferSize is automatically increased");
            parameters.bufferSize = 1;
        }
        if (parameters.delay <= 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure delay > 0");
            console.log(nodeName+" object: parameter delay is automatically increased");
            parameters.maxDelay = 0.001;
        }
        if (parameters.delay > (parameters.bufferSize - 1)*timeStep)
        {
            console.exception(nodeName+" object: delay is too high and exceeds actual buffer size");
            console.log(nodeName+" object: parameter delay is automatically decreased to (bufferSize - 1)*timeStep = "+(parameters.bufferSize - 1)*timeStep);
            parameters.delay = (parameters.bufferSize - 1)*timeStep;
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize uHistory
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initHistory()
    {
        //var fruits = [];
        //fruits.push("banane", "pomme", "pêche");
        //console.log(typeof(fruits));
        //fruits.prototype.fill("tata");

        //console.log(fruits);

        //console.log(typeof(uHistory));
        // nombre de cellules dans l'historique: 10 000
       // uHistory.fill(input.value);
        //for (var i=0;i< bufferSize;i++)
        //        uHistory.prototype.push(input.value)


        statesvar.uHistory = new Array(parameters.bufferSize);
        for (var k = 0; k < parameters.bufferSize; k++) {
            (statesvar.uHistory)[k] =  input.value;
        }

       //console.log(nodeName, " : de type FixedDelay; taille de la mémoire  " + (statesvar.uHistory).length);
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
           //console.log("Debug delay", statesvar.uHistory);
        }

       var mem_old = input.value;
       var mem_new;
       for (var k = 1; k < parameters.bufferSize; k++) {
           mem_new = (statesvar.uHistory)[k];
           (statesvar.uHistory)[k] =  mem_old;
           mem_old = mem_new;
       }

       var index = 2;
       index = Math.round(parameters.delay / (timeStep));

//       if (nodeName == "TdMinusOneHr") {
//       console.log("Debug delay: history",index, nodeName, statesvar.uHistory);
//}

       if (index > parameters.bufferSize - 1) index = parameters.bufferSize - 1;

        output.value = (statesvar.uHistory)[index];
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{checkParaConsistency()}
}
