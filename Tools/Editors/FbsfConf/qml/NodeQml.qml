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
//        color: "purple"
//        width: 120
//        height: 120
//        radius: style.radiusmodule
//        border.width: style.border


        color: style.colorNode
        radius: style.radiusmodule
        border.width: style.border
        width: style.widthModule
        height: style.heigthModule
//        x: 150
//        y: 0
        Text {
            id: name
            text: "Node"
            x:parent.width/2 - 20
            y: 10
            font.pointSize: 15
        }
//        Button {
//                text: "DEL"
//                onClicked: mydestroynext(modu, idnode)
//                y: parent.width - 40
//            }

//        Button {
//                text: "div"
//                onClicked: {
//                    var newidnode = spaceManager.addChildModules(idnode);
//                    createModel(modu, modu.list, newidnode, "ModuleQml.qml");
//                }

//                y: parent.width - 40
//                x: 50
//            }

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
                    text: "Delete Node !"
                    onTriggered: mydestroyNode(modu, idnode, endNode)
                }
            }
        }

//        ButtonAddModule {
//            pare: modu
//        }
//        ButtonAddNode {
//            pare: modu
//        }
        Drop {
            pare: modu
        }
        DropDiv {
            pare: modu
        }
    }
}
