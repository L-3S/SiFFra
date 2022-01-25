import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3

import "../Common.js" as Def

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ item data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ListView {id: dataList

    property var checkFct
    property bool enableVerticalScrollBar: true

    width: parent.width
    implicitHeight: contentItem.childrenRect.height
    boundsBehavior: Flickable.StopAtBounds
    headerPositioning: ListView.OverlayHeader
    rightMargin: 10
    clip: true
    layer.enabled: true
    ScrollBar.vertical: ScrollBar {
        id: scBa
        active: enableVerticalScrollBar
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
    header:
        Rectangle {
        z:10
        height:40
        width : parent.width
        color: Def.btNavBgColor
        Row {
            z: 10
            bottomPadding: 14
            visible: (dataList.model && dataList.model.length ===0) ?
                         false : true
            //~~~~~~~~~~~~~~~~ Name key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Text {
                leftPadding: 10
                z: 10
                text            : "Parameter" ;
                width           : Def.nameColumWidth;
                font.bold       : Def.ParamHeader_Stw.bold
                font.pixelSize  : Def.ParamHeader_Stw.size
                font.family     : Def.ParamHeader_Stw.family
                color           : Def.ParamHeader_Stw.color
                anchors.verticalCenter: parent.verticalCenter
            }
            //~~~~~~~~~~~~~~~~ value field ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Text {
                z: 10
                text            : "Value"
                width           : Def.valueColumWidth
                font.bold       : Def.ParamHeader_Stw.bold
                font.pixelSize  : Def.ParamHeader_Stw.size
                font.family     : Def.ParamHeader_Stw.family
                color           : Def.ParamHeader_Stw.color
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment:Text.AlignHCenter
            }
            //~~~~~~~~~~~~~~~~ unit field ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Text {
                z: 10
                text            : "Unit"
                width           : Def.unitColumWidth
                font.bold       : Def.ParamHeader_Stw.bold
                font.pixelSize  : Def.ParamHeader_Stw.size
                font.family     : Def.ParamHeader_Stw.family
                color           : Def.ParamHeader_Stw.color
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment:Text.AlignHCenter
            }
            //~~~~~~~~~~~~~~~~ Optional, Addtional or Mpndatory field ~~~~
            Text {
                z: 10
                text            : "Type"
                width           : Def.typeColumWidth
                font.bold       : Def.ParamHeader_Stw.bold
                font.pixelSize  : Def.ParamHeader_Stw.size
                font.family     : Def.ParamHeader_Stw.family
                color           : Def.ParamHeader_Stw.color
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment:Text.AlignHCenter
            }
            //~~~~~~~~~~~~~~~~ help ~~~~
            Text {
                z: 10
                text: "Help"
                width           : Def.unitColumWidth
                font.bold       : Def.ParamHeader_Stw.bold
                font.pixelSize  : Def.ParamHeader_Stw.size
                font.family     : Def.ParamHeader_Stw.family
                color           : Def.ParamHeader_Stw.color
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment:Text.AlignHCenter
            }
        }
    }

    delegate: Column{
        width: dataList.width;
        spacing : 10
        Row {
            opacity: dataList.enabled?1:0.5
            topPadding:3
            //~~~~~~~~~~~~~~~~ Name key ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Text {
                id: nameField
                property var nameContent: model.modelData.value
                leftPadding: 10
                text            : model.modelData.key + " :"
                width           : Def.nameColumWidth-(btOpen.visible
                                                      ||btCalendar.visible
                                                      ||btClock.visible?
                                                          btOpen.width:0)
                font.bold       : Def.Param_Stw.bold
                font.pixelSize  : Def.Param_Stw.size
                font.family     : Def.Param_Stw.family
                color:(typeField.text!=="M" || (nameContent!=="" && nameContent!==undefined)) ?
                          Def.Param_Stw.color :"orange"
                wrapMode        : Text.WordWrap
                anchors.verticalCenter: parent.verticalCenter
            }
            ToolButton {id:btOpen
                visible : model.modelData.type==='path'||model.modelData.type==='filepath'
                icon.name   : "Open"
                icon.source : "qrc:/icons/fileopen.png"
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Open configuration")
                onClicked   : {
                    fileDialog.selectFolder=model.modelData.type==='path'?true:false
                    fileDialog.target=loader.item
                    fileDialog.nameFilters=[model.modelData.unit]
                    fileDialog.open()
                }
            }
            ToolButton {id:btCalendar
                visible : model.modelData.type==='date'
                icon.name   : "Calendar"
                icon.source : "qrc:/icons/calendar.png"
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Open calendar")
                onClicked   : {
                    // set selected_date to textfield date if not empty
                    if(model.modelData.unit!=="")
                        datePicker.dateFormat=model.modelData.unit //set date format from unit
                    datePicker.itemText=loader.item
                    datePicker.itemModel=model.modelData // to set unit as date format
                    datePicker.open()
                }
            }
            ToolButton {id:btClock
                visible : model.modelData.type==='time'
                icon.name   : "Clock"
                icon.source : "qrc:/icons/clock.png"
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Open clock setter")
                onClicked   : {
                    // set selected_date to textfield date if not empty
                    if(model.modelData.unit!=="")
                        timePicker.timeFormat=model.modelData.unit //set date format from unit
                    timePicker.itemText=loader.item
                    timePicker.itemModel=model.modelData // to set unit as date format
                    timePicker.open()
                }
            }
            //~~~~~~~~~~~~~~~~ value field ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Loader {id : loader
                property var xData:model.modelData
                property var nameFieldV: nameField
                property var typeFieldV: typeField
                width : Def.valueColumWidth
                sourceComponent: {
                    if(xData===null) return
                    if( xData.type==='int'
                            ||xData.type==='number'
                            ||xData.type==='string'
                            ||xData.type==='path'
                            ||xData.type==='filepath'
                            ||xData.type==='DateAndTime'
                            ||xData.type==='DateAndTimeUTC'
                            ||xData.type==='date'
                            ||xData.type==='time'
                            )
                        return textInput
                    else if(xData.type==='bool')
                        return booleanChoice
                    else if(xData.type==='checkable')
                        return booleanChekable
                    else if(xData.type==='list')
                        return listChoice
                    //                    else if(xData.type==='choiceList')
                    //                        return choiceList
                }
            }
            //~~~~~~~~~~~~~~~~ unit field ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Text {
                text            : model.modelData.unit
                width           : Def.unitColumWidth
                font.bold       : Def.Param_Stw.bold
                font.pixelSize  : Def.Param_Stw.size
                font.family     : Def.Param_Stw.family
                color           : Def.Param_Stw.color
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment:Text.AlignHCenter
                wrapMode        : Text.WordWrap
            }
            //~~~~~~~~~~~~~~~~ Optional or Mandatory field ~~~~
            Text {
                id: typeField
                text: model.modelData.optional ?
                          "O" : model.modelData.mandatory ? "M":""
                width           : Def.unitColumWidth
                font.bold       : Def.Param_Stw.bold
                font.pixelSize  : Def.Param_Stw.size
                font.family     : Def.Param_Stw.family
                color           : Def.Param_Stw.color
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment:Text.AlignHCenter
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
                    var minmaxInfo=
                            (  (model.modelData.type==='int'
                                || model.modelData.type==='number')
                             && model.modelData.minStrict!==undefined
                             && model.modelData.minWarn!==undefined)?
                                "\n : MinStrict = "+model.modelData.minStrict
                                +", MaxStrict = "+model.modelData.maxStrict
                                +"\n : MinWarn = "+model.modelData.minWarn
                                +", MaxWarn = "+model.modelData.maxWarn
                              :""
                    helpMessage.hlpText =model.modelData.description + minmaxInfo
                    helpMessage.open()
                    keyboard.focus=true
                }
            }
            Image{
                visible : model.modelData.hasError
                width : 16 ; height : width
                source : "qrc:/icons/warning.png"
                anchors.verticalCenter: parent.verticalCenter
                MouseArea{anchors.fill:parent;
                    hoverEnabled:true;
                    onEntered:{ tooltipValidator.text=model.modelData.error;
                                tooltipValidator.visible=true}
                }
            }
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Text input component
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component{
        id : textInput

        TextField {
            id : textInputField
            width: parent.width
            height: Def.ParamVal_Stw.size + 30

            text: (xData!==null && xData.value!==undefined)?setText():""
            font.bold       : Def.ParamVal_Stw.bold
            font.pixelSize  : Def.ParamVal_Stw.size
            font.family     : Def.ParamVal_Stw.family
            color           : Def.ParamVal_Stw.color
            horizontalAlignment:Text.AlignHCenter
            Component.onCompleted: textInputField.ensureVisible(0)
            background: Rectangle {
                radius: 2
                width: parent.width
                height: parent.height
                anchors.fill: parent
                color: Def.panelColor
                border.color:xData.hasError?"red": Def.frameColor
                border.width: xData.hasError?3:1
            }
            // function to check date validity
            function setText()
            {
                if(xData.type==='date' && xData.value!=="")
                {
                    if(xData.isoDate instanceof Date && !isNaN(xData.isoDate))
                    {
                        // get the iso date if valid
                        datePicker.selected_date=xData.isoDate
                        // set the formatted date
                        text=xData.value
                    }
                }
                if(xData.type==='time' && xData.value!=="")
                {
                    timePicker.selected_time=xData.value
                }

                return xData.value
            }

            selectByMouse   : true
            property string previousText: (xData!==null && xData.value!==undefined)?xData.value:""
            onEditingFinished:{
                textInputField.ensureVisible(0)

                if (previousText === text)
                {
                    // Nothing changed so don't trigger inspector modification
                    return
                }
                // Check number range validity
                else if(xData.type==='int' || xData.type==='number')
                {
                    if(Number(text)<xData.minStrict||Number(text)>xData.maxStrict)
                    {
                        tooltipValidator.text=xData.key+" Value out of range ["+xData.minStrict+","+xData.maxStrict+"]"
                        tooltipValidator.visible=true
                        text=previousText
                        return
                    }
                }

                xData.value=text
                previousText=text
                nameFieldV.nameContent=text
                inspector.modified(xData.key,xData.value)
                // get the iso date if valid
                if(xData.type==='date') datePicker.selected_date=xData.isoDate
            }
            //~~~~~~~~~~~~~~~ Validator section ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            validator :(xData!== null && xData.type==='number') ?
                           numberValidator
                         : (xData!== null && xData.type==='bool') ?
                               boolValidator
                             : (xData!== null && xData.type==='int') ?
                                   intValidator
                                 : null

            DoubleValidator { id: numberValidator;  locale:"C";
                notation: DoubleValidator.ScientificNotation
            }
            IntValidator { id: intValidator}
            RegExpValidator { id: boolValidator;    regExp: /true|false|TRUE|FALSE/ }

            //Date and time validator : yyyy MM dd HH:mm:ss format
            //Date and Time (format UTC e.g. 2015-03-25T12:00:00Z)
            inputMask: (xData.type==='DateAndTime')?"9999 99 99 99:99:99"
                          : (xData.type==='DateAndTimeUTC')?"9999-99-99T99:99:99Z"
                             :(xData.type==='date')?"9999-99-99"
                                 :(xData.type==='time')?"99:99:99"
                                                       :null
            inputMethodHints: (   xData.type==='DateAndTime'
                               || xData.type==='DateAndTimeUTC'
                               || xData.type==='date'
                               || xData.type==='time')?
                                  Qt.ImhDigitsOnly:Qt.ImhNone
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // List component
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component{
        id : listChoice
        ComboBox {
            model:(xData!==null && xData.value!==undefined)?xData.value:[]
            //width : Def.valueColumWidth
            height: Def.ParamVal_Stw.size + 30

            onActivated: {
                xData.index=currentIndex;
                inspector.modified(xData.key,xData.value[currentIndex])
            }
            currentIndex: xData!==null?xData.index:0

            anchors.verticalCenter: parent.verticalCenter

            font.bold       : Def.ParamVal_Stw.bold
            font.pixelSize  : Def.ParamVal_Stw.size
            font.family     : Def.ParamVal_Stw.family
            background: Rectangle {
                radius: 2
                width: parent.width
                height: parent.height
                anchors.fill: parent
                color: Def.panelColor
                border.color:xData.hasError?"red": Def.frameColor
                border.width: xData.hasError?3:1
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // boolean Choice component
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component{
        id : booleanChoice
        ComboBox {
            model:["true","false"]
            displayText:xData.value
            //width : Def.valueColumWidth
            height: Def.ParamVal_Stw.size + 30

            onActivated: {
                xData.value=currentText;
                displayText=currentText;
                inspector.modified(xData.key,xData.value)
            }
            anchors.verticalCenter: parent.verticalCenter

            font.bold       : Def.ParamVal_Stw.bold
            font.pixelSize  : Def.ParamVal_Stw.size
            font.family     : Def.ParamVal_Stw.family
            background: Rectangle {
                radius      : 2
                width       : parent.width
                height      : parent.height
                anchors.fill: parent
                color       : Def.panelColor
                border.color:xData.hasError?"red": Def.frameColor
                border.width: xData.hasError?3:1
            }
        }
    }
    Component{
        id : booleanChekable
        Row {
            spacing: 10
            height: Def.ParamVal_Stw.size + 30

            CheckBox {
                id: checkboxId
                height: parent.height
                width: height
                checked : (xData!==null && xData.value!==undefined)?
                              (xData.value==="true"):false
                onClicked:
                {
                    xData.value=checked
                    inspector.modified(xData.key,xData.value)
                }
            }
        }
    }


    // Help Message
    HelpMessage {
        id: helpMessage
    }
    //~~~~~~~~~~~~~~ Validator tooltip ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ToolTipValidator{id:tooltipValidator}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FileDialog{
        id: fileDialog
        folder : controller.getAppHome()
        selectExisting: true
        property var target
        onAccepted:{
            target.text=controller.getRelativePath(fileUrl)
            target.editingFinished()
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Date chooser
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    DatePicker{
        id:datePicker
        property var itemText
        property var itemModel
        onDateChanged: {
            itemText.text=date
            itemText.editingFinished()
            inspector.modified(itemModel.key,itemModel.value)
        }
        onDateFormatChanged: {
            if(itemModel!==undefined) itemModel.unit=dateFormat // could be empty
            // if date is set apply format to text field
            // CHANGE : ISO 8601 format, Do not change the date text
//            if(itemText!==undefined && itemText.text!=="")
//            {
//                itemText.text=selected_date.toLocaleDateString(Qt.locale(), format)
//                itemText.editingFinished()
//            }
            inspector.modified("unit",dateFormat)
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Time chooser
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    TimePicker{
        id:timePicker
        property var itemText
        property var itemModel
        onTimeChanged: {
            itemText.text=time
            itemText.editingFinished()
            inspector.modified(itemModel.key,itemModel.value)
            console.log(time)
        }
        onTimeFormatChanged: {
            if(itemModel!==undefined) itemModel.unit=timeFormat // could be empty
            inspector.modified("unit",timeFormat)
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Choice component
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //    Component{
    //        id: choiceList

    //        Row{        Component.onCompleted: console.log(xData.value)

    //            width: parent.width
    //            height: listviewId.height +20
    //            topPadding: 10
    //            //~~~~~~~~~~~~~~~~ Choice list~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //            Rectangle{
    //                id: choiceRectangle
    //                color: Def.panelColor
    //                radius: 2
    //                border.color: Def.frameColor
    //                border.width: 1
    //                height : (listviewId.height != 0 ? listviewId.height+10 : 45)
    //                width : listviewId.width
    //                ListView {
    //                    id: listviewId
    //                    width: Def.valueColumWidth
    //                    height: childrenRect.height
    //                    model:(xData!==null && xData.value!==undefined)?
    //                              xData.value:[]
    //                    delegate: Item {
    //                        width: parent.width
    //                        height: 30
    //                        Row {
    //                            spacing: 10
    //                            anchors.fill: parent
    //                            anchors.margins: 5
    //                            CheckBox {
    //                                id: checkboxId
    //                                height: parent.height
    //                                width: height
    //                                checked : (xData!==null && xData.value!==undefined)?
    //                                              xData.isChoiceSelected(index):false
    //                                onClicked:
    //                                {
    //                                    modified()
    //                                    xData.selectedChoice(index,checked)
    //                                }
    //                            }
    //                            Label {
    //                                text: modelData
    //                                width: parent.width - checkboxId.width
    //                                height: parent.height
    //                                font.pixelSize:     Def.Param_Stw.size
    //                                font.family:        Def.Param_Stw.family
    //                                anchors.verticalCenter: checkboxId.verticalCenter
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
}
