import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12

import "../Common.js" as Def

Window{
    id      : inspector
    visible : true
    x       : 0
    y       : 0
    width   : 1000
    height  : itemType==="config"?1000:600
    color   : Def.panelColor
    title   : itemName
    onClosing: closeCallBack()

    property int    leftPadding     : 20

    signal inspectorClosed()

    // Update list when param list changed
    property var updatedModel
    onUpdatedModelChanged:{
        paramList.model=updatedModel
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted:
    {
        panel.dataModel=itemParamsModel
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function modified(key,value) {
        controller.itemParamChanged(index,key,value)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Column{
        id : panel
        property var objVwidth : Def.nameColumWidth + Def.valueColumWidth + 3 * Def.unitColumWidth /// the ObjectView width
        width : objVwidth
        height: parent.height
        topPadding: 10
        property alias dataModel : paramList.model
        ToolTipValidator{id:tooltipValidator}

        //~~~~~~~~~~~~ item name ~~~~~~~~~~~~~~~~~~~~
        Row{
            id:title
            bottomPadding: 5
            Text{
                leftPadding : 10
                text        : "Name :";
                width       : Def.nameColumWidth;
                anchors.verticalCenter: title.verticalCenter

                font.bold:          Def.ParamHeader_Stw.bold
                font.pixelSize:     Def.ParamHeader_Stw.size
                font.family:        Def.ParamHeader_Stw.family
                color:              Def.ParamHeader_Stw.color
            }
            TextField{
                text            : itemName
                selectByMouse   : true
                width           : Def.valueColumWidth
                anchors.verticalCenter: title.verticalCenter
                readOnly        : itemType==="config"
                layer.enabled   : true
                font.bold       : Def.ParamVal_Stw.bold
                font.pixelSize  : Def.ParamVal_Stw.size
                font.family     : Def.ParamVal_Stw.family
                color           : Def.ParamVal_Stw.color
                horizontalAlignment : TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignBottom

                background: Rectangle {
                    radius  : 2
                    width   : parent.width
                    height  : parent.height
                    anchors.fill: parent
                    color   : Def.panelColor
                    border.color: Def.frameColor
                    border.width: 1
                }

                property string previousText: itemName
                onEditingFinished:
                {
                    if (previousText === text)// Nothing changed so don't trigger modification
                        return
                    else
                    {
                        var ret = itemParent.setName(text)
                        if (ret === 0) {// valid case
                            previousText = text
                        } else if (ret === 1) {
                            tooltipValidator.text="Item Name must be unique"
                            tooltipValidator.visible=true
                            text=previousText // set it back
                            return
                        } else if (ret === 2) {
                            tooltipValidator.text="Item Name can't be empty"
                            tooltipValidator.visible=true
                            text=previousText // set it back
                            return
                        }
                    }
                }
            }
            //~~~~~~~~~~~~~~~~ Help Button (with pre and post spacers) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Rectangle{
                color:"transparent"
                height: Def.Param_Stw.size
                width: (Def.unitColumWidth - helpButton.width) / 2
            }
            ToolButton {
                id: helpButton
                icon.color  : Def.btBgColor
                icon.source : "qrc:/icons/help.png"
                anchors.verticalCenter: parent.verticalCenter
                onClicked   : {
                    helpMessage.hlpText = "help for " + itemCategory
                    helpMessage.open()
                    keyboard.focus=true
                }
            }
            Rectangle{
                color:"transparent"
                height: Def.Param_Stw.size
                width: (Def.unitColumWidth - helpButton.width) / 2
            }
        }
        //~~~~~~~~~~~~ type of Module ~~~~~~~~~~~~~~~~~~~~
        Row{
            id:typeModule
            bottomPadding       : 5
            visible : itemType==="module"||itemCategory==="plugin"
            Text{
                leftPadding     : 10
                text            : "Type :"
                width           : Def.nameColumWidth;
                anchors.verticalCenter: typeModule.verticalCenter
                font.bold       : Def.ParamHeader_Stw.bold
                font.pixelSize  : Def.ParamHeader_Stw.size
                font.family     : Def.ParamHeader_Stw.family
                color           : Def.ParamHeader_Stw.color
            }
            ComboBox {
                width   : Def.valueColumWidth
                editable: false
                enabled : itemCategory!=="plugin"
                model   : controller.moduleTypeList
                textRole: "typeCpp"
                displayText: itemCategory!=="plugin"?itemModuleType:itemCategory
                onActivated:
                {
                    itemParent.setModuleType(currentText)
                    displayText=currentText;
                }
            }
        }
        // dynamic list
        ObjectView{id:paramList; height: panel.height - title.height - 10}
    }

    // Help Message (currently not defined in modules)
    HelpMessage {
        id: helpMessage
    }
}
