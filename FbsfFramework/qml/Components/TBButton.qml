import QtQuick 2.0

BorderImage {
    id: button

    signal clicked
    property bool highlighted : false
    property string label :""

    width: height; height: parent.height
    //clip : true
    smooth : true

    onEnabledChanged: {state=enabled?"enabled":"disabled"}
    // display tooltip for plugin
    Tooltip{id:tooltip;text:label;anchors.top:button.bottom}
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {button.clicked()}
        hoverEnabled: true
        onEntered   : tooltip.show()
        onExited    : tooltip.hide()
    }
    Rectangle   {width : parent.height ; height : parent.height;
                 visible:button.highlighted;radius : 4
                 color:"transparent";border.color:"red";border.width:2}
    states:
    [
        State{
            name: "disabled"
            PropertyChanges { target: button; opacity: .4 }
        },
        State{
            name: "enabled"
            PropertyChanges { target: button; opacity: 1 }
        },
        State{
            name: "pressed"; when:mouseArea.pressed === true
            PropertyChanges { target: button;
                border {left: -3; top: -3; right: -3; bottom: -3 }}
        }
    ]
}
