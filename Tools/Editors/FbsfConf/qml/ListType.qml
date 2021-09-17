import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls.Styles 1.4

ListView {
    id:sublist
    interactive: true
    //                    y: 50
    //                    property var myname: parent.myname
    model: managetype.getSize();
    width: 200
    height: windowSimulation.height
    visible: true
    delegate: ItemDelegate {
        id: subitem
        property int indexOfThisDelegate: index
        property int savx: 0
        property int savy: 0

        width: parent.width
        height: 50
        text: managetype.getByIndex(index)
        //                        icon.source: "file:" + dirApi.getImageFiles(myname, index)
        icon.color: "transparent"
        icon.width: 40
        icon.height: 40


        Rectangle {
            border.color: "grey"
            border.width: 1
            anchors.fill: parent
            color: "transparent"

        }
        Drag.active: dragArea.drag.active
        Drag.hotSpot.x: 100
        Drag.hotSpot.y: 25
        MouseArea {
            id: dragArea
            anchors.fill: parent

            drag.target: parent

            onPressed: {
                subitem.savx = subitem.x;
                subitem.savy = subitem.y;
                console.log("PRESS")
                setvisibilitydrag(false, index);
            }

            onReleased: {
                subitem.x = savx;
                subitem.y = savy;
                console.log("released")
                setvisibilitydrag(true, index);

            }
        }

    }

    Component.onCompleted: windowSimulation.racList = sublist;
}
