import QtQuick 2.0
import QtQuick.Controls 2.14

Item {
    id: modu
    width: 100
    height: 100
    x: 150
    y: 0
    property var list: []
    property var idnode: 0
    property var nb:""
    property var endNode: ""
    Rectangle {
        id: uhuh
        color: style.colorMachine
        radius: style.radiusmodule
        border.width: style.border
        width: style.widthModule
        height: style.widthModule
        Text {
            id: name
            text: "Machine" + nb
            x:parent.width/2 - 40
            y: 10
            font.pointSize: 15
        }

        Button {
            id: but
            text: "Edit"
            property var list: []
            x : parent.width/2 - width/2
            y : parent.height - 45
            onClicked: menu.open()
            Menu {
                id: menu

                MenuItem {
                    text: "New Sequence"
                    onTriggered: {
                        var newidnode = spaceManager.addChildModulessesq(idnode);
                        createSequence(modu, modu.list, newidnode);
                    }
                }
                MenuItem {
                    text: "Delete Machine !"
                    onTriggered: mydestroynext(modu, idnode)
                }
            }
        }
    }
}
