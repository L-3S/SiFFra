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
//        color: "maroon"
//        width: 120
//        height: 120
//        radius: style.radiusmodule
//        border.width: style.border


        color: style.colorEndNode
        radius: style.radiusmodule
        border.width: style.border
        width: style.widthModule
        height: style.widthModule
        Text {
            id: name
            text: "End"
            x:parent.width/2 - 20
            y: 10
            font.pointSize: 15
        }
//        ButtonAddModule {
//            pare: modu
//        }
//        ButtonAddNode {
//            pare: modu
//        }

        Button {
            id: but
            text: "Edit"
            property var list: []
            onClicked: menu.open()
            x : parent.width/2 - width/2
            y : parent.height - 45
            Menu {
                id: menu

                MenuItem {
                    text: "New Node"
                    onTriggered: {
                        addNode(modu, idnode);
                    }
                }
            }
        }
        Drop {
            pare: modu
        }
    }
}
