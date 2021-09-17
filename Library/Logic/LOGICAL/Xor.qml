import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

PartialBoolDISO
{
    nodeType: "Xor"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        output.value = false;
        for (var i=0;i< socketsLeft.length;i++)
        {
            if (socketsLeft[i].value)
                output.value ^= socketsLeft[i].value;

        }

    }
}
