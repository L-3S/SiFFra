import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

PartialIntSI
{
    nodeType: "IntOutput"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Notification of a command value
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property int mCommand: NaN
    SignalInt{tag1:nodeName;value:mCommand}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        mCommand = input.value
    }



}

