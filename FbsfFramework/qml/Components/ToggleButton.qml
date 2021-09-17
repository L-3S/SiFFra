//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Component : a toggle button.
//
//  Public properties :
//
//          onImageSource : icon for selected state
//
//  Signal thrown :
//
//          selected ()
//          unselected()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
import QtQuick 2.0

BorderImage {
    id: toggleButton
    width   :150
    height  :50
    property url onImageSource
    property alias imageSource: buttonImage.source
    property string label :""

    signal selected()
    signal unselected()

    state: "unPressed"
    onStateChanged: {if (state == "pressed") {selected();} }

    Image
    {
        id: buttonImage
        smooth: true
        anchors.fill: parent
        onStatusChanged: if (buttonImage.status === Image.Error)
                             source = "qrc:///qml/Components/icons/button_missing.png"
    }
    // display tooltip for plugin
    Tooltip{id:tooltip;text:label;bgcolor:"white";anchors.top:parent.bottom}

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        onPressed: {if (parent.state == "unPressed")  { unselected()}}
        hoverEnabled: true
        onEntered: tooltip.show()
        onExited: tooltip.hide()
    }

    states: [
        State {
            name: "pressed"
            PropertyChanges {
                target: toggleButton
                opacity : .4
                imageSource: onImageSource
                border {left: 5; top: 5; right: 5; bottom: 5 }
            }

        },
        State {
            name: "unPressed"
            PropertyChanges {
                target: toggleButton
                opacity : 1
                imageSource: onImageSource
            }
        }
    ]
}
