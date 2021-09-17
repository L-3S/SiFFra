import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12

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
        color: style.colorModule
        radius: style.radiusmodule
        border.width: style.border
        width: style.widthModule
        height: style.widthModule
        Text {
            id: name
            text: spaceManager.getInfosType(idnode)
            x:parent.width/2 - 20
            y: 10
            font.pointSize: 15
        }
        MouseArea {
                anchors.fill: parent
                onClicked:createPopupInspector(idnode);
//                onClicked:racinspector.createInfoInspector(idnode);
            }

//        Button {
//                text: "DEL"
//                onClicked: {
//                    modu.parent.list = mydestroy(modu, idnode);
//                    racinspector.checkDel(idnode);
//                }
//                y: parent.width - 40
//            }



//        ButtonAddModule {
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
                    text: "Delete this module !"
                    onTriggered: {
                        modu.parent.list = mydestroy(modu, idnode);
                        racinspector.checkDel(idnode);
                    }
                }
            }
        }
//        ButtonAddNode {
//            pare: modu
//        }
        Drop {
            pare: modu
        }
    }


    function createPopupInspector(idnode) {
        var component;
        var savcomponent;

        component = Qt.createComponent("PopupInspector.qml");
        savcomponent = component.createObject(windowSimulation, {idnode:idnode});
        return savcomponent;
    }
}
