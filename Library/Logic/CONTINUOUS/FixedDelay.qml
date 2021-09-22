import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSISO {
    nodeType: "FixedDelay"
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
    // maxDelay: maxDelayTime
    property var parameters : {"delay" : 18}




    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check parameters consistency
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function checkParaConsistency()
    {
        // TESTING AND COMPUTING TO DO ONLY AT INIT OR
        // AFTER PARAMETER CHANGE BY USER IN EDITOR
        // Consistence des paramètres d'entrée
        if (parameters.delay <= 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure delay > 0");
            console.log(nodeName+" object: parameter delay is automatically increased");
            parameters.maxDelay = 0.001;
        }
        if (parameters.delay > (bufferSize - 1)*timeStep)
        {
            console.exception(nodeName+" object: delay is too high and exceeds actual buffer size");
            console.log(nodeName+" object: parameter delay is automatically decreased to (bufferSize - 1)*timeStep = "+(bufferSize - 1)*timeStep);
            parameters.delay = (bufferSize - 1)*timeStep;
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

        while ((statesvar.uHistory).length <   bufferSize)
        {(statesvar.uHistory).push(input.value)}

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
        }

       // ajoute un élément au début du tableau
       (statesvar.uHistory).unshift(input.value);
       // supprime le dernier élément du tableau
       (statesvar.uHistory).pop();

       var index = 2;
       index = Math.round(parameters.delay / (timeStep));

       if (index > bufferSize - 1) index = bufferSize - 1;

        output.value = (statesvar.uHistory)[index];
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{checkParaConsistency()}



}
