import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialBoolSO
{
    nodeType: "Hysteresis"
    source  : nodeType+".svg"
	
    socketsLeft: [
        Socket {id: u;socketId: "u";direction: "input";type: "real"}
                ]

    // set parameters
    property var parameters : {"uLow" :0., "uHigh" : 1.}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check parameters consistency
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{checkParaConsistency()}
    function checkParaConsistency()
    {
        if (parameters.uHigh - parameters.uLow < 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure uHigh >= uLow");
            console.log(nodeName+" object: parameter uHigh is automatically increased to uLow value");
            parameters.uHigh = parameters.uLow;
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set state variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // outputMem: state value for y
    // initialise and defined during mFirstStep
    // outputMem should be stored in snapshots "State Variable"
    //property var variables : {'outputMem' : false}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
             statesvar.mFirstStep = false;
             statesvar.outputMem = false;
         }

        output.value = (!statesvar.outputMem && (u.value > parameters.uHigh)) || (statesvar.outputMem && (u.value >= parameters.uLow));
        statesvar.outputMem = output.value;
    }
}
