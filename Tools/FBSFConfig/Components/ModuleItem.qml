import QtQuick 2.12
import QtQml.Models 2.12

Element {
    id : base
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Graphic item for module
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    width   : itemRow.width + itemShape.width
    height  : itemRow.height

    Rectangle {
        id      : itemShape
        width   : text.paintedWidth+10
        height  : grapher.moduleHeight

        color       : category===""?"lightgrey":"white"
        border.color: highlight ? "red" : category===""?"orange":"white"
        border.width: highlight ? 3 : category===""?3:1
        radius      : 10

        // module name
        Text{id:text
            text    : name
            color   : category===""?"red":"black"
            font.bold:  category===""
            font.pixelSize: textPixelSize
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter  : parent.verticalCenter
        }
        // type of module
        Text{id:typeSimu
            text    : moduleType
            color   : "white"
            font.pixelSize: 12
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom  : parent.top
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
                    var footerText=""
                    if (type=="module")
                    {
                        if(category=="plugin")
                             footerText=category + " " + name
                        else
                            footerText=type + " " + name + " Type : " + moduleType
                    }
                    setSelected(footerText)
                }
                else
                {
                    if(menuEnabled)
                    {
                        if(category!="plugin")
                        {
                            moduleMenu.rightEnabled=index < parentListview.count-1
                            moduleMenu.leftEnabled=(index > (parentListview.objectName==="rootList"?1:0))
                            moduleMenu.removeEnabled=parentListview.count > 1
                            moduleMenu.popup()
                        }
                        else
                        {
                            pluginMenu.rightEnabled=index < parentListview.count-1
                            pluginMenu.leftEnabled=(index > 0)
                            pluginMenu.removeEnabled=parentListview.count>1
                            pluginMenu.popup()
                        }
                    }
                }
            }
            onDoubleClicked: {if (mouse.button == Qt.LeftButton) showInspector()}
        }
    }
}
