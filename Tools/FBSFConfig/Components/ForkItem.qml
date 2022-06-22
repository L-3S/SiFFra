import QtQuick 2.12
import QtQml.Models 2.12
Element {
    id : base
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Graphic shape for fork
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    width   : itemRow.width + hSpacing + itemShape.width
    height  : itemRow.height + sequenceHeight/2
    property color forkColor : category==="rootFork"?"#1f89d9":"#0ccfa5"
    property color frameColor : category==="rootFork"?"#1f89d9":"#0ccfa5"

    Rectangle{
        id          : itemShape
        width       : category==="rootFork"?forkWidth*2:forkWidth
        x           : -width
        y           : moduleHeight/2-sequenceHeight/2-3
        height      : base.height
        color       : highlight ? Qt.lighter(forkColor,1.5) : forkColor
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
                    setSelected("group : " + name)
                }
                else // diplay popup menu
                {
                    if(menuEnabled && selected)
                    {
                        forkBeginMenu.removeGroupEnabled=(category!=="rootFork")
                        forkBeginMenu.rightEnabled=index < parentListview.count-1
                        forkBeginMenu.leftEnabled=(index > 0)
                        forkBeginMenu.popup()
                    }
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // ForkEnd delimiter
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Rectangle{id:forkEnd
            width       : itemShape.width
            height      : itemShape.height
            color       : itemShape.color
            border.color: itemShape.border.color
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
                        if(menuEnabled && selected && category!=="rootFork")
                        {
                            forkEndMenu.rightEnabled=index < parentListview.count-1
                            forkEndMenu.leftEnabled=(index > 0)
                            forkEndMenu.popup()
                        }
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
        color   : frameColor//"#217780"
        border.color: "yellow"
        visible : highlight
        y       : itemShape.y
        width   : itemRow.width+itemShape.width+forkWidth+hSpacing
        height  : itemShape.height
    }
}
