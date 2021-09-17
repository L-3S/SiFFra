import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

PartialDISO
{
    nodeType: "Add"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yInternal: internal value for y
    property var variables : {'yInternal' : 0}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // k1 : gain for input 1
    // k2 : gain for input 2
    property var parameters : {"k1" :1, "k2" : 1 }

    // Dynamic Display of the parameter in the document
    Text{
        id:parameterk1Value;
        anchors.bottom: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 13; anchors.bottomMargin: 8;
        font.pointSize:6;color: "red";horizontalAlignment : Text.AlignVCenter;

    }
    Text{
        id:parameterk2Value;
        anchors.top: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 13;anchors.topMargin: 8;
        font.pointSize:6;color: "red";horizontalAlignment : Text.AlignVCenter;
    }

    function updateDisplay()
    {
        parameterk1Value.text = "k1: " + parameters.k1;
        parameterk2Value.text = "k2: " + parameters.k2;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
        updateDisplay();
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        variables.yInternal = input1.value * parameters.k1 + input2.value * parameters.k2;
        output.value = variables.yInternal;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{updateDisplay()}
}

