import QtQuick 2.12
import QtQuick.Controls 2.14
import "../Common.js" as Def

Popup {
    id: helpMessage
    property alias hlpText:helpText.text
    width: 500
    Rectangle{
        anchors.fill: parent
        color: "transparent"
        border.color: Def.frameColor
        border.width: 1
        implicitHeight: helpText.implicitHeight
        Text {
            id: helpText
            anchors.fill: parent
            padding: 20
            font.bold:          Def.Help_Stw.bold
            font.pixelSize:     Def.Help_Stw.size
            font.family:        Def.Help_Stw.family
            color:              Def.Help_Stw.color
            wrapMode: Text.WordWrap
        }
    }
}
