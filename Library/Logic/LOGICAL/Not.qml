import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

PartialBoolSISO
{
    nodeType: "Not"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        output.value = ! input.value
    }
}
