import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

// NO STATES (except simulation time)

PartialSO
{
    nodeType: "SawTooth"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yInternal: internal value for y
    property var variables : {'yInternal' : 0}


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // amplitude: amplitude of SawTooth --- default = 1
    // period: period of SawTooth (in s) --- default = 3600
    // nperiod: Number of periods (< 0 means infinite number of periods) --- default = -1
    // offset : off set of output signal --- default = 1
    // startime : Output = offset for time < startTime --- default = 0
    property var parameters : {"amplitude" : 1,
                               "period" : 3600,
                               "nperiod" : -1,
                               "offset" : 0,
                               "starttime" : 0 }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check parameters consistency
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function checkParaConsistency()
    {
        // TESTING AND COMPUTING TO DO ONLY AT INIT OR
        // AFTER PARAMETER CHANGE BY USER IN EDITOR
        // Consistence des paramètres d'entrée
        if (parameters.period <= 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure period is strictly positive");
            console.log(nodeName+" object: parameter period is automatically increased");
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

        var reltime = statesvar.mlocSimuTime - parameters.starttime;
        var sawtoothDuration = (parameters.nperiod > 0) ? parameters.nperiod * parameters.period : 1e38;

        variables.yInternal = 0;

        if (reltime > 0 &  reltime < sawtoothDuration) {
            variables.yInternal = parameters.amplitude * reltime / parameters.period;
            variables.yInternal -= parameters.amplitude * Math.floor(reltime / parameters.period);
        }

        variables.yInternal += parameters.offset;

        output.value = variables.yInternal;

        statesvar.mlocSimuTime += timeStep;
        //console.log(nodeName+" object: local simutime and timestep", statesvar.mlocSimuTime, timeStep);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{checkParaConsistency()}


}
