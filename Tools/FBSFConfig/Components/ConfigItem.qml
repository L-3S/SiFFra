import QtQuick 2.12
import QtQml.Models 2.12

Element {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Graphic shape for config
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    width   : itemRow.width + itemShape.width
    height  : itemRow.height + hSpacing * 1.5
    x       : parentListview.count===1?0:-20 // avoid shift right

    Component.onCompleted: {
        itemRow.x+=itemShape.width+hSpacing
    }
    Rectangle {
        id      : itemShape
        y       : grapher.moduleHeight/2-grapher.configHeight/2
        width   : (textName.paintedWidth+10>configWidth?
                      textName.paintedWidth+10:configWidth)
        height  : grapher.configHeight

        color       : "#1f89d9"
        border.color: highlight ? "red" : "white"
        border.width: highlight ? 3 : 1
        radius      : 10
        Image{
            anchors.left : itemShape.left
            anchors.top  : itemShape.bottom
            width : 16 ; height : width
            source : "qrc:/icons/warning.png"
            visible : hasError
        }
        // configuration name
        Text{id:textName
            text    : name
            color   : "white"
            font.pixelSize: textPixelSize
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter  : parent.verticalCenter
        }
        Connections{
            target : controller.config
            function onConfigNameChanged()
            {
                if(grapher.isCurrent)
                    textName.text=controller.config.configName
            }
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
                    setSelected(type + " " + name)
                }
                else
                {
                    if(menuEnabled && !controller.config.hasPluginList)
                        configMenu.popup()
                }
            }
            onDoubleClicked: {if (mouse.button == Qt.LeftButton) showInspector()}
        }
    }
}

