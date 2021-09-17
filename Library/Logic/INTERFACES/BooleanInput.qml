import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

PartialBoolSO
{
    nodeType: "BooleanInput"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subscription to a external value
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property int mValue: 0
    SubscribeInt{
        tag1: nodeName;
        onValueChanged: {mValue = value;} }

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
        if (mValue == 1) {output.value = true}
        else {output.value = false}
    }
}

