import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node
{

    source : onoff ? "SwitchOn.png":"SwitchOff.png"


    property bool onoff : false
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var parameters : {"ValName":""}
    property var statesvar  : {"onoff":onoff}
    Component.onCompleted:{
        output.tag1=parameters.ValName
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property real mSignal: 0
    SignalInt{id:output;value: mSignal}
    MouseArea
    {id: actMouse
        // Case Time Depends no action form actuators alowed
        enabled: !simuMpc
        anchors.fill : parent
        onClicked:
        {
            onoff=!onoff
            mSignal=onoff ? 1:0
        }
    }
    function stateSaving()
    {
        statesvar.onoff=onoff
    }
    function stateRestored()
    {
        onoff=statesvar.onoff
        mSignal=onoff ? 1:0
    }
}

