import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// NO STATES

PartialSISO
{
    nodeType: "DeadZone"
    source  : nodeType+".svg"
	
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var parameters : {"uMin" :-1E38, "uMax" : 1E38 }

    onParameterModified :{checkParaConsistency(name)}
    function checkParaConsistency(paramName)
    {
        if (parameters.uMax - parameters.uMin < 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure uMax >= uMin");
            console.log(nodeName+" object: parameter uMax is automatically increased to uMin value");
            parameters.uMax = parameters.uMin;
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
       if (input.value > parameters.uMax & input.value >= parameters.uMin)
           output.value = input.value - parameters.uMax;
       else if (input.value < parameters.uMin)
           output.value = input.value - parameters.uMin;
       else
           output.value = 0;
    }

}

