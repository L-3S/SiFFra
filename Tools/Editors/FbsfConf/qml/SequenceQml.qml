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
//        color: "orange"
//        width: 120
//        height: 120
//        radius: style.radiusmodule
//        border.width: style.border

        color: style.colorSequence
        radius: style.radiusmodule
        border.width: style.border
        width: style.widthModule
        height: style.widthModule
//        x: 150
//        y: 0
        Text {
            id: name
            text: "Seq"
            x:parent.width/2 - 20
            y: 10
            font.pointSize: 15
        }

//        Button {
//                text: "DEL"
//                onClicked: mydestroynext(modu, idnode)
//                y: parent.width - 40
//            }

//        ButtonAddModule {
//            pare: modu
//        }
        Drop {
            pare: modu
        }

//        ButtonAddNode {
//            pare: modu
//        }

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
                    text: "New Node"
                    onTriggered: {
                        addNode(modu, idnode);
                    }
                }
                MenuItem {
                    text: "Delete Sequence !"
                    onTriggered: mydestroynext(modu, idnode)
                }
            }
        }

    }


}
