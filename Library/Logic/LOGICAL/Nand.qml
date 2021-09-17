import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

PartialBoolMISO
{
    nodeType: "Nand"
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        var andVar = true ;
        for (var i=0;i< socketsLeft.length;i++)
                andVar &= socketsLeft[i].value;
        output.value = !andVar;
    }
}
