import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// NO STATES

PartialRealBoolSISO
{
    nodeType: "FormulaRIBO"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var expressions : {"Formula" : "x>simulationTime"}
    property var myFonc   // function helper

    // La formule ne peut pas être modifiée en dynamique --> il n'est pas ncessaire de prévoir des états

    // Dynamic Display of the eqaution in the document
    Text{
        id:formulaTxt;
        anchors.horizontalCenter: parent.horizontalCenter;anchors.top: parent.bottom;
        anchors.topMargin: 10;
        font.pointSize:12;color: "red";horizontalAlignment : Text.AlignVCenter;
    }

    onExpressionModified:
    {
        if(name==="Formula")
        {
            myFonc=new Function('x','simulationTime', ('return (' + expr + ');'));
            formulaTxt.text = expressions.Formula;
        }
    }

    function initialize()
    {
        myFonc=new Function('x','simulationTime', ('return (' + expressions.Formula + ');'));
        formulaTxt.text = expressions.Formula;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        if (statesvar.mFirstStep){
             statesvar.mFirstStep = false;
         }

        output.value = myFonc(input.value,simulationTime);
    }

}

