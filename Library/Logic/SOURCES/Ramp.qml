import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

// NO STATES (except simulation time)

PartialSO
{
    nodeType: "Ramp"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yInternal: internal value for y
    property var variables : {'yInternal' : 0}


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yStart : start value during at initComput
    // k : constant output value
    property var parameters : {"offset" : 0, "duration" : 1, "height" : 1, "starttime" : 0 }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check parameters consistency
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function checkParaConsistency()
    {
        // TESTING AND COMPUTING TO DO ONLY AT INIT OR
        // AFTER PARAMETER CHANGE BY USER IN EDITOR
        // Consistence des paramètres d'entrée
        if (parameters.duration <= 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure duration is strictly positive");
            console.log(nodeName+" object: parameter duration is automatically increased");
            parameters.duration = 1E-5;
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
        checkParaConsistency();
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
            statesvar.mlocSimuTime = 0;
        }

        //variables.yInternal = parameters.offset+(parameters.height)/parameters.duration*(simulationTime-parameters.starttime)
        variables.yInternal = parameters.offset+(parameters.height)/parameters.duration*(statesvar.mlocSimuTime-parameters.starttime)


        var lowBound = 0.0;
        var highBound = 1.0;
        lowBound = Math.min(parameters.offset,parameters.offset+parameters.height);
        highBound = Math.max(parameters.offset,parameters.offset+parameters.height);
        if (variables.yInternal < lowBound){
            variables.yInternal = lowBound;
        }
        if (variables.yInternal > highBound){
            variables.yInternal = highBound;
        }

        output.value = variables.yInternal;

        statesvar.mlocSimuTime += timeStep;
        //console.log(nodeName+" object: local simutime and timestep", statesvar.mlocSimuTime, timeStep);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{checkParaConsistency()}

}

