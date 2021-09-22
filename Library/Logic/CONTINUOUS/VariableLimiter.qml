import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// NO STATES

PartialSISO {
    nodeType: "VariableLimiter"
    source  : nodeType+".svg"

    // uMin, uMax : bound values for output
    socketsLeft: [
        Socket {id: uMax;socketId: "uMax";direction: "input";type: "real"},
        Socket {id: uMin;socketId: "uMin";direction: "input";type: "real"}
                ]

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        if (input.value >= uMax.value)
            output.value = uMax.value;
        else if (input.value <= uMin.value)
            output.value = uMin.value;
        else
            output.value = input.value;
    }
}
