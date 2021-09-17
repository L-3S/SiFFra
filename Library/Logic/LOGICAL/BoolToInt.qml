import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

PartialBoolIntSISO {

    nodeType: "BoolToInt"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
        }

        output.value = (input.value === true) ? 1 : 0;
    }
}
