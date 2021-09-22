import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

PartialMISO
{
    nodeType: "Max"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        output.value = socketsLeft[0].value;
        for (var i=1;i< socketsLeft.length;i++)
        {
                if ( output.value < socketsLeft[i].value)
                    output.value =socketsLeft[i].value;
        }
    }
}

