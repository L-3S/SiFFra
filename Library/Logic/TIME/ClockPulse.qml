import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

// STATES INTEGRATED
// a local time is mandatory for the timeDependMode -> introduction of statesvar.mlocSimuTime

PartialBoolSO
{
    nodeType: "ClockPulse"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // startime : in s
    // period : in s
    property var parameters : {"starttime" :0, "period" : 86400 }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // timeNew: clock time with respect to starttime (in s)
    property var variables : {'timeNew' : 0}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set state variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // timeOld: state value

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
        statesvar.timeOld = 0.;
        statesvar.mlocSimuTime = 0.;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        if (statesvar.mFirstStep){
             statesvar.mFirstStep = false;
             statesvar.timeOld = parameters.starttime-timeStep;
             statesvar.mlocSimuTime = parameters.starttime;
         }

        variables.timeNew = (statesvar.mlocSimuTime-parameters.starttime)%(parameters.period);

        output.value = (variables.timeNew < statesvar.timeOld) ? true : false

        statesvar.timeOld = variables.timeNew;
        statesvar.mlocSimuTime += timeStep;
    }

}
