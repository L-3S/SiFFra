import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12
import QtQuick.Dialogs 1.3

ToolBar {
    id: appBar
    height: 48
    width: rootApp.width
    property alias buttonCheck:btCheck
    RowLayout {
        width: parent.width
        //~~~~~~~~~~~~~~~~~~~~~~~~~ Toolbar project ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        ToolButton {id: btNew
            icon.source : "qrc:/icons/filenew.png"
            opacity     : enabled ? 1 : 0.3
            ToolTip.visible: hovered
            ToolTip.text: qsTr("New configuration")
            padding     : 10
            action      : newAction
        }
        Action {
            id: newAction
            enabled     : !controller.config.modified
            shortcut: StandardKey.New
            onTriggered: { rootApp.newConfig();keyboard.focus=true  }
        }
        ToolButton {id:btOpen
            icon.name   : "Open"
            icon.source : "qrc:/icons/fileopen.png"
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Open configuration")
            opacity     : enabled ? 1 : 0.3
            action      : openAction
        }
        Action {
            id: openAction
            enabled     : !controller.config.loaded && !controller.config.modified
            shortcut: StandardKey.Open
            onTriggered: {
                fileDialog.title="Open"
                fileDialog.open()
                keyboard.focus=true
            }
        }
        ToolButton {id:btClose
            icon.source : "qrc:/icons/fileclose.png"
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Close configuration")
            opacity     : enabled ? 1 : 0.3
            action      : closeAction
        }
        Action {
            id: closeAction
            shortcut: StandardKey.Close
            enabled     : controller.config.loaded
            onTriggered: {
                keyboard.focus=true
                rootApp.closeConfig();
            }
        }
        ToolButton {id:btSave
            icon.source : "qrc:/icons/filesave.png"
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Save configuration")
            opacity     : enabled ? 1 : 0.3
            action      : saveAction
        }
        Action {
            id: saveAction
            enabled     : controller.config.loaded && controller.config.modified
            shortcut    : StandardKey.Save
            onTriggered : {
                rootApp.saveConfig()
                keyboard.focus=true
            }
        }
        ToolButton {id:btSaveAs
            icon.name   : "Save as"
            icon.source : "qrc:/icons/filesaveas.png" // TODO icon
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Save configuration as")
            opacity     : enabled ? 1 : 0.3
            action      : saveAsAction
        }
        Action {
            id: saveAsAction
            enabled     : controller.config.loaded||controller.config.modified
            shortcut: StandardKey.SaveAs
            onTriggered: {
                rootApp.saveConfigAs()
                keyboard.focus=true
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~ Toolbar verif ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //verification Tools
        ToolSeparator{}
        ToolButton {id:btCheck
            icon.name   : "Check"
            icon.source : "qrc:/icons/check.png"
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Check configuration")
            enabled     : controller.config.loaded && (controller.config.hasError || controller.config.modified)
            opacity     : enabled ? 1 : 0.3
            onClicked   : {
                keyboard.focus=true
                rootApp.checkConfig()
            }
        }

        //~~~~~~~~~~~~~~~~~~~~~~~~~ Toolbar edit ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //Edition Tools
        ToolSeparator{}
        Row
        {
            id:editButtons
            spacing: 10

        }
        ToolSeparator{}
        Rectangle{
            height: 48
            Layout.fillWidth: true
            color: "transparent"
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~ Toolbar help ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //Exit et Help Tools
        ToolSeparator{}
        ToolButton {
            icon.source : "qrc:/icons/help.png"
            onClicked   : {
                Qt.openUrlExternally(FBSF_HOME+"Documentation/pdf/MutConfigEditor.pdf");
                keyboard.focus=true
            }
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Help")
        }
        ToolButton {
            icon.source : "qrc:/icons/exit.png"
            onClicked   : { quit() ;keyboard.focus=true}
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Quit editor")
        }
        Rectangle{
            height: 48
            width: 20
            color: "transparent"
        }
    }
}
