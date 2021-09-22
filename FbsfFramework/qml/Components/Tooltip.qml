import QtQuick 2.1

Rectangle {
    id: toolTip
    property alias text : toolTipText.text
    property alias fontSize : toolTipText.font.pixelSize
    property color bgcolor : "white"
    property alias timer : showTimer
    property bool display: false
    width: toolTipText.width + 15
    height:toolTipText.height + 15
    color: bgcolor
    border.color: "#0a0a0a"
    radius: 5
    z:100
    function show(){timer.start()}
    function showDuring(msec){display = true;hideTimer.interval=msec;hideTimer.start()}
    function hide(){timer.stop();display = false; }
    opacity: text != "" && display ? 1 : 0

    Text {
        id: toolTipText
        color: "black"
        font.pixelSize : 12
        anchors.centerIn: parent
    }

    Behavior on opacity {
        PropertyAnimation {
            easing.type: Easing.InOutQuad
            duration: 250
        }
    }

    Timer {
        id: showTimer
        interval: 1000
        onTriggered: {display = true;}
    }
    Timer {
        id: hideTimer
        interval: 1000
        onTriggered: {display = false;}
    }
}
