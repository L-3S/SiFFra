import QtQuick 2.0
import QtQuick.Shapes 1.15

Element {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Graphic shape for plugin list
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    id      : base
    y       : rootSequence.height+hSpacing  // set y below root sequence
    width   : 0 // stick ConfigItem to left
    height  : itemRow.height + 15
    Component.onCompleted: {
        itemRow.x=sequenceWidth + hSpacing
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // base line for the plugin list
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rectangle{
        id      : line
        z       : -10
        y       : moduleHeight/2
        width   : itemRow.width+hSpacing
        height  : 2
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Background frame
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rectangle{ id: sequenceFrame
        z       : line.z-1
        color   : "#6068a2"
        border.color: "yellow"
        visible : highlight
        y       : itemShape.y
        width   : itemRow.width+1.5*hSpacing
        height  : moduleHeight
        // Plugin list name
        Text{
            id              : txtName
            text            : "Plugins"
            color           : "lightblue"
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
            fillColor   : highlight ? "gold" : "blue"

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
                    setSelected(name)
                }
                else
                {
                    if(menuEnabled && selected)
                    {
                        pluginsMenu.popup()
                    }
                }
            }
        }
    }
}
