import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSISO {
    nodeType: "FirstOrder"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yNew: internal value for y
    property var variables : {'yNew' : 0}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set state variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yOld: state value for y
    // initialise and defined during mFirstStep


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // k : gain
    // tDer: time constant for Derivative term
    property var parameters : {"k" : 1,
                               "tDer" : 0.1}

//    property var locPara  :{"initialEquation": true};

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check parameters consistency
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function checkParaConsistency()
    {
        // TESTING AND COMPUTING TO DO ONLY AT INIT OR
        // AFTER PARAMETER CHANGE BY USER IN EDITOR
        // Consistence des paramètres d'entrée
        if (parameters.tDer <= 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure tDer > 0");
            console.log(nodeName+" object: parameter tDer is automatically increased");
            parameters.tDer = 0.001;
        }
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
    function compute()
    {
       if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
            statesvar.yOld = input.value;
        }
       //  der(y) = k*(u - y)/tDer;
       // (yN-yO)/timeStep = k*(u - yN)/tDer;
       // fmr = k*timestep/tDer;
       // yN = yO + fmr * (u - yN)
       // yN * (1+fmr) = (yO + fmr * u)

        var fmr = timeStep/parameters.tDer*parameters.k;
        variables.yNew = (statesvar.yOld + fmr * input.value)/(1+fmr);

        output.value = variables.yNew;

        // Update Old variables
        statesvar.yOld = variables.yNew;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{checkParaConsistency()}

}
