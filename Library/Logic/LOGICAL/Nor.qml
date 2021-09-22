import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

PartialBoolMISO
{
    nodeType: "Nor"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        output.value = true;
        for (var i=0;i< socketsLeft.length;i++)
        {
            if (socketsLeft[i].value)
            {
                output.value = false;
                return;
            }
        }
    }
}
