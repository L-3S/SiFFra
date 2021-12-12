import QtQuick 2.11
import QtQuick.Controls 2.15
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3
import "Common.js" as Def

ApplicationWindow {
    id:rootApp
    visible: true
    title: (controller.config.modified?"FBSFConfig* ":"FBSFConfig ")
                                       + controller.config.configUrl
    width : 1200
    height : 600
    header: AppToolBar{id:toolBar}
    color                       : "Dimgrey"
    property real zoomFactor          : 1
    property real zoomFactorMin       : 0.1
    property real zoomFactorMax       : 2

    property var currentConfigEditor : configEditor1.isCurrent?
                                           configEditor1:configEditor2

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // close project and quit Application
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onClosing:{close.accepted =false; quit() }
    function quit()
    {
        if(controller.config.modified) msgQuit.visible=true
        else  Qt.quit()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function newConfig()
    {
        if(controller.config.loaded===true) closeConfig()
        controller.newConfig()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function openConfig(aFileName)
    {
        controller.openConfig(aFileName)
        controller.checkConfig()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function saveConfig()
    {
        msgCheck.report=controller.checkConfig()
        if(msgCheck.report.length===0)
            controller.saveConfig()
        else
            msgCheck.visible=true
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function saveConfigAs2(url)
    {
        msgCheck.report=controller.checkConfig()
        if(msgCheck.report.length===0)
            controller.saveConfigAs(url)
        else
            msgCheck.visible=true
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function saveConfigAs()
    {
        msgCheck.report=controller.checkConfig()
        if(msgCheck.report.length===0)
        {
            fileDialog.title="Save as"
            fileDialog.open()
        }
        else
        {
            msgCheck.visible=true
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function closeConfig()
    {
        if(controller.config.modified) msgClose.visible=true
        else
        {
            controller.closeConfig()
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function checkConfig()
    {
        msgCheck.report=controller.checkConfig()
        if(msgCheck.report.length===0)
            msgCheck.report="Configuration is valid"
        msgCheck.visible=true
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setFooterText(text)
    {
        footerText.text=text
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setCurrentConfig(num)
    {
        if(num===1)configEditor2.resetSelection();
        else configEditor1.resetSelection();
        controller.setCurrentConfig(num)
        configEditor1.isCurrent=num===1?true:false
        configEditor2.isCurrent=!configEditor1.isCurrent
        keyboard.focus=true
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    RightToolBar{}
    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical
        //~~~~~~~~~~~~~~~~~~~~~~ Config 1 UI layers ~~~~~~~~~~~~~~~~~~~~~~~~~~
        ScrollView {
            id : configArea1
            SplitView.minimumHeight : 100
            SplitView.fillHeight    : true
            clip: true
            contentWidth    : configEditor1.width
            contentHeight   : configEditor1.height
            property ScrollBar hScrollBar: ScrollBar.horizontal
            property ScrollBar vScrollBar: ScrollBar.vertical
            // Selection rectangle
            Rectangle {
                x            : configArea1.hScrollBar.position*configArea1.contentWidth
                y            : configArea1.vScrollBar.position*configArea1.contentHeight
                width        : configArea1.width
                height       : configArea1.height
                color        : rootApp.color
                border.width : configEditor1.isCurrent? 3:0
                border.color : configEditor1.isCurrent? "lime":"black"
                MouseArea{
                    anchors.fill : parent
                    acceptedButtons: Qt.LeftButton
                    preventStealing: true
                    propagateComposedEvents: true
                    onWheel : {
                        if (wheel.angleDelta.y > 0)
                        {   //zoom in
                            if(zoomFactor<zoomFactorMax) zoomFactor*=1.1
                        }
                        else
                        {   // zoom out
                            if(zoomFactor>zoomFactorMin) zoomFactor*=0.9
                        }
                        keyboard.focus=true
                    }
                    onClicked: {
                        configEditor1.resetSelection();
                        configEditor2.resetSelection();
                        setFooterText("")
                        setCurrentConfig(1)}
                }
            }
            FBSFConfig{
                id: configEditor1
                transformOrigin : Item.TopLeft
                isCurrent       : true
                treeModel       : controller.config1
                transform       :
                    Scale { origin.x: 0; origin.y: 0
                    xScale: zoomFactor;yScale:zoomFactor}
                function itemSelected() {setCurrentConfig(1)}

            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~ Config 1 UI layers ~~~~~~~~~~~~~~~~~~~~~~~~~~
        ScrollView {
            id : configArea2
            clip: true
            SplitView.minimumHeight: 20
            SplitView.preferredHeight: 20
            SplitView.maximumHeight: rootApp.height/2
            contentWidth    : configEditor2.width
            contentHeight   : configEditor2.height
            property ScrollBar hScrollBar: ScrollBar.horizontal
            property ScrollBar vScrollBar: ScrollBar.vertical
            // Selection rectangle
            Rectangle {
                x            : configArea2.hScrollBar.position*configArea2.contentWidth
                y            : configArea2.vScrollBar.position*configArea2.contentHeight
                width        : configArea2.width
                height       : configArea2.height
                color        : rootApp.color
                border.width : configEditor2.isCurrent? 3:0
                border.color : configEditor2.isCurrent? "lime":"black"
                MouseArea{
                    anchors.fill : parent
                    acceptedButtons: Qt.LeftButton
                    preventStealing: true
                    propagateComposedEvents: true
                    onWheel : {
                        if (wheel.angleDelta.y > 0)
                        {   //zoom in
                            if(zoomFactor<zoomFactorMax) zoomFactor*=1.1
                        }
                        else
                        {   // zoom out
                            if(zoomFactor>zoomFactorMin) zoomFactor*=0.9
                        }
                        keyboard.focus=true

                    }
                    onClicked: {
                        configEditor1.resetSelection();
                        configEditor2.resetSelection();
                        setFooterText("")
                        setCurrentConfig(2)}
                }
            }
            FBSFConfig{
                id: configEditor2
                transformOrigin : Item.TopLeft
                treeModel       : controller.config2
                transform       :
                    Scale { origin.x: 0; origin.y: 0
                    xScale: zoomFactor;yScale:zoomFactor}
                function itemSelected() { setCurrentConfig(2)}
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Handle keyboard input
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Item {
        id      :keyboard
        focus   : true
        enabled : true
        property  bool  ctrlPressed: false
        property  bool  shiftPressed: false
        Keys.onPressed:
        {
            if (event.modifiers & Qt.ControlModifier)
            {
                ctrlPressed=true    // multi selection
                if (event.key === Qt.Key_X) {
                    currentConfigEditor.cutSelection();
                    event.accepted = true;
                }
                else if (event.key === Qt.Key_C) {
                    currentConfigEditor.copySelection();
                    event.accepted = true;
                }
                else if (event.key === Qt.Key_V) {
                    currentConfigEditor.pasteSelection()
                    event.accepted = true;
                }
                else if (event.key === Qt.Key_Z) {
                    currentConfigEditor.undo()
                    event.accepted = true;
                }
                else if (event.key === Qt.Key_Y) {
                    currentConfigEditor.redo()
                    event.accepted = true;
                }
            }
            else if (event.modifiers & Qt.ShiftModifier)
            {
                shiftPressed=true    // extended selection
            }
            else
            {
                switch (event.key)
                {
                case Qt.Key_Delete : currentConfigEditor.removeSelection()
                }
            }
        }
        Keys.onReleased:
        {
            if (!(event.modifiers & Qt.ControlModifier))
            {
                ctrlPressed=false   // multi selection
            }
            if (!(event.modifiers & Qt.ShiftModifier))
            {
                shiftPressed=false    // extended selection
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Dialog {
        id      : msgQuit
        title   : "FBSF configurator Warning"
        standardButtons : Dialog.Save | Dialog.Discard | Dialog.Cancel
        Label {
            color : "black"
            text  : "The configuration have been modified.\nUnsaved changes will be lost.
                    Do you want to save ?"
        }
        onAccepted: { rootApp.saveConfig();}
        onDiscard: { Qt.quit() }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Dialog {
        id      : msgClose
        title   : "FBSF configurator Warning"
        standardButtons : Dialog.Save | Dialog.Discard | Dialog.Cancel
        Label {
            color : "black"
            text  : "The configuration have been modified.\nUnsaved changes will be lost.
                    Do you want to save ?"
        }
        onAccepted: { rootApp.saveConfig();}
        onDiscard: {
            controller.config.modified=false
            closeConfig()
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    CheckReport{id : msgCheck}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FileDialog{
        id: fileDialog
        nameFilters: ["XML files (*.xml)"]
        selectFolder:false
        selectExisting: (title==="Open" && title!=="Save as")
        onAccepted:{
            if(title==="Open")
            {
                openConfig(fileUrl)
            }
            else // Save as
            {
                saveConfigAs2(fileUrl)
            }
        }
    }
    footer :
        Rectangle{id: footer
        width : parent.width
        height : 30
        color : Def.panelColor
        Text  {id : footerText
            color : "white"
            font.pixelSize  : Def.fontSize
            anchors.verticalCenter: parent.verticalCenter
            anchors.left : parent.left
            anchors.leftMargin: 20
        }
    }
}

