import QtQuick 2.7
import Grapher 1.0
import "../PARTIAL"

// NO STATES

PartialRealBoolDISO
{
    nodeType: "GreaterEqual"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
        }

        output.value = (input1.value >= input2.value) ? true : false
    }
}
