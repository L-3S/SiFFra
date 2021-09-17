import QtQuick 2.7
import Grapher 1.0
import "../PARTIAL"

// NO STATES

PartialIntDynaMISO
{
    nodeType: "MeanInt"

    shape.width : 100
    shape.height : socketContainerLeft.height

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // AddMode : boolean
    // AddMode = true: simple add
    //           else: mean value
    property var parameters : {"AddMode" :false }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
            statesvar.uintOld = 1;
        }

        var nbSocketIn = socketsLeft.length;
        var fmr = 0;

        for (var i=0;i<nbSocketIn;i++)
        {
            fmr += socketsLeft[i].value;
        }

        var divider = (parameters.AddMode) ? 1 : nbSocketIn;

        fmr /= divider;

        // WARNING: force integer output
        fmr = Math.round(fmr);

        output.value = fmr;
    }
}
