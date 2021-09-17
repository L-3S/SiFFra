import QtQuick 2.12
import QtQuick.Controls 2.2

ToolTip{
    id:tooltipValidator
    delay:100
    timeout: 3000
    contentItem: Text {
        text: tooltipValidator.text
        font.pointSize: 10;color: "lightyellow"
    }
    background: Rectangle {color: "#e28d62"}
}
