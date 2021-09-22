import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// NO STATES

PartialBoolMISO
{
    nodeType: "And"
//    source  : nodeType+".svg"

    property bool abool: true;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        abool = true;

        for (var i=0;i< socketsLeft.length;i++)
            abool = ( abool & socketsLeft[i].value);

        output.value = abool;
    }
}
