import QtQuick 2.12
import QtQml.Models 2.12
Element {
    id : base
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Graphic shape for fork
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    width   : itemRow.width + hSpacing + itemShape.width
    height  : itemRow.height + sequenceHeight/2
    Rectangle{
        id          : itemShape
        width       : forkWidth
        x           : -forkWidth
        y           : moduleHeight/2-sequenceHeight/2-3
        height      : base.height
        color       : highlight ? "lime" : "#0ccfa5"
        border.color: highlight ? "red" : "#113"

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // item selection
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        MouseArea {
            id : mouseArea
            anchors.fill: itemShape
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked:{
                if (mouse.button === Qt.LeftButton)
                {
                    setSelected("")
                }
                else
                {
                    forkBeginMenu.popup()
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // ForkEnd delimiter
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Rectangle{id:forkEnd
            width       : 5
            height      : itemShape.height
            color       : itemShape.color
            anchors.verticalCenter: itemShape.verticalCenter
            anchors.left: itemShape.right
            anchors.leftMargin:  itemRow.width + 1.5*hSpacing
            MouseArea {
                anchors.fill: forkEnd
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked:{
                    if (mouse.button === Qt.LeftButton)
                    {
                        setSelected("")
                    }
                    else
                    {
                        if(menuEnabled && category!=="rootFork")
                            forkEndMenu.popup()
                    }
                }
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Background frame
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rectangle{ id: forkFrame
        z       : -10
        color   :"#217780"
        border.color: "yellow"
        visible : highlight
        y       : itemShape.y
        width   : itemRow.width+itemShape.width+forkWidth+hSpacing
        height  : itemShape.height
    }
}
