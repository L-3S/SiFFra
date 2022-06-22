import QtQuick 2.0
import QtQuick.Shapes 1.15

Element {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Graphic shape for sequence
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    id      : base
    width   : itemRow.width + itemShape.width
    height  : itemRow.height + 15
    Component.onCompleted: itemRow.x+=itemShape.width+hSpacing
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // base line for the sequence
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rectangle{
        id      : line
        z       : -10
        y       : moduleHeight/2
        width   : parentRow.width+1.5*hSpacing
        height  : 2
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Background frame
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rectangle{ id: sequenceFrame
        z       : line.z-1
        color   :"#6068a2"
        border.color: "yellow"
        visible : highlight
        y : itemShape.y
        width   : parentRow.width+1.5*hSpacing
        height  : moduleHeight
        // sequence name
        Text{
            id              : txtName
            text            : name
            color           : "lightgreen"
            font.pixelSize  : 12
            font.bold       : true
            anchors.left    : parent.left
            anchors.top     : parent.bottom
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Item's graphic shape
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Shape{id:itemShape
        containsMode            : Shape.FillContains
        width                   : sequenceWidth
        height                  : sequenceHeight
        property int dx         : sequenceWidth
        property int dy         : sequenceHeight

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // arrow shape
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        ShapePath {id:arrowPath
            strokeWidth : highlight ? 2:1
            strokeColor : highlight ? "red" : "white"
            fillColor   : highlight ? "gold" : "green"

            startX      : itemShape.dx  // pointe droite
            startY      : line.y        // pointe droite

            // The graphical path
            PathLine { relativeX: -itemShape.dx ; relativeY: itemShape.dy/2}    //oblique bas
            PathLine { relativeX: 0 ; relativeY : -itemShape.dy}                // vertical
            PathLine { x: arrowPath.startX ; y: arrowPath.startY}               // oblique haut
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // item selection
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        MouseArea {
            id : mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked:{
                if (mouse.button == Qt.LeftButton)
                {
                    setSelected(type + " : " + name
                                + " (" + params[0].key + " : " + params[0].value +")")
                }
                else
                {
                    if(menuEnabled && selected)
                    {
                        sequenceMenu.removeEnabled=parentListview.count>1
                        sequenceMenu.downEnabled=index<parentListview.count-1
                        sequenceMenu.upEnabled=index>0
                        sequenceMenu.popup()
                    }
                }
            }
            onDoubleClicked: {if (mouse.button == Qt.LeftButton) showInspector()}
        }
    }
}
