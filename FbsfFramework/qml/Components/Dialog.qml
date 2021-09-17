import QtQuick 2.0
import QtQuick.Dialogs 1.1

MessageDialog {
    id: messageDialog
    title: ""
    text: ""
    modality: Qt.WindowModal

    icon : title === "ERROR"    ? StandardIcon.Critical:
                     "WARNING"  ? StandardIcon.Warning:
                     "Save"     ? StandardIcon.Question:
                                  StandardIcon.Information
    standardButtons: title === "Save" ? StandardButton.Save|StandardButton.Cancel:
                                        StandardButton.Ok
    onAccepted: {}
    onRejected: {}


    Component.onCompleted:
    {
        console.log("[",title,"]",text)
        visible = true
    }
}
