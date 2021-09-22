import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3

import Grapher 1.0
import fbsfplugins 1.0

Node
{
    property var barWidth: 0
    property real barValue: 0
    property real minBar: parameters.relativeDisplay ? -parameters.saturation : (parameters.threshold - parameters.saturation)
    property real maxBar: parameters.relativeDisplay ? parameters.saturation : (parameters.threshold + parameters.saturation)
    property real centerBar: parameters.relativeDisplay ? "0" : parameters.threshold

    /////////////////////////////////////////////////
    // Inspector parameters:
    /////////////////////////////////////////////////
    property var parameters: {
        "title": "My Bargraph Title",
        "varName": "","nbdigit": 1,
        "saturation": 10.,"threshold": 12., "relativeDisplay": false,
        "negColor": "red","posColor": "green", "bgColor": "gray", "txtColor": "white","txtSize": 10,
        "horizontal": true, "width": 400, "height": 60
    }

    /////////////////////////////////////////////////
    // Update Parameters
    /////////////////////////////////////////////////
    Component.onCompleted:{
        parametersChanged()
    }

    /////////////////////////////////////////////////
    // Subscribes in standart or SimuMPC mode
    /////////////////////////////////////////////////
    SubscribeReal{id: inputScalar
        // In SimuMPC mode this variable is not needed
        tag1 : simuMpc ? "" : parameters.varName
        onValueChanged: {
            if (parameters.relativeDisplay )
                barValue = value - parameters.threshold
            else
                barValue = value

            if (barValue < minBar || barValue > maxBar )
                barWidth = 0.5 * scaleRec.width
            else
                barWidth =  Math.abs((barValue - centerBar) * scaleRec.width / (maxBar-minBar))
        }
    }
    SubscribeVectorReal {id: inputVector
        // In normal mode this variable is not needed
        tag1 : simuMpc ? parameters.varName  : ""
        onDataChanged: {
            // Case simuMpc mode get last value of the past data[pastSize-1]
            if (parameters.relativeDisplay )
                barValue = data[pastSize-1] - parameters.threshold
            else
                barValue = data[pastSize-1]

            if (barValue < minBar || barValue > maxBar )
                barWidth = 0.5 * scaleRec.width
            else
                barWidth =  Math.abs((barValue - centerBar) * scaleRec.width / (maxBar-minBar))
        }
    }

    //Control the shape of the Node model
    textName.visible : false
    shape.visible : false
    shape.width : parameters.width
    shape.height : parameters.height
    shape.rotation: parameters.horizontal ? 0:-90

    Rectangle {
        width: parameters.width
        height: parameters.height
        border.color    : "black"
        border.width    : 2
        radius: 5
        color: parameters.bgColor
        rotation: parameters.horizontal ? 0:-90
        //Bargraph Title
        Text {
            visible: parameters.horizontal
            text: parameters.title
            anchors.bottom: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 15
            font.pixelSize: parameters.txtSize+2
            font.bold: true
        }
        Text {
            visible:!parameters.horizontal
            text: parameters.title
            anchors.right: parent.left
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: parameters.txtSize+2
            font.bold: true
            anchors.margins: 15 - 0.5 *width
            rotation: 90
        }

        // Bargraph Value
        Text {
            text: barValue.toFixed(parameters.nbdigit)
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.right
            font.pixelSize: parameters.txtSize + 4
            color: barValue > centerBar ? parameters.posColor : parameters.negColor
            rotation: parameters.horizontal ? 0:90
            anchors.margins: parameters.horizontal ? 10: 10 - 0.5 * width

        }

        // Bar and Scale
        Item {
            anchors.fill: parent
            anchors.margins: 10
            // Dynamic Bar
            Rectangle {
                height: parent.height
                width: barWidth
                anchors.left: barValue > centerBar ? parent.horizontalCenter : undefined
                anchors.right: barValue < centerBar ? parent.horizontalCenter : undefined
                color: barValue > centerBar ? parameters.posColor : parameters.negColor
            }
            // Graduation
            RowLayout {id:scaleRec
                anchors.fill: parent
                spacing: -1
                Repeater {
                    model: 20
                    Rectangle {
                        Layout.fillWidth: parent
                        Layout.fillHeight: parent
                        border.width: 1
                        border.color: parameters.txtColor
                        color: "transparent"
                    }
                }
            }
        }
        // Scale Values
        // Min Value
        Text {
            anchors.top:parent.bottom
            anchors.left: parent.left
            text: minBar.toFixed(parameters.nbdigit)
            color: barValue <= minBar ? "red" : parameters.txtColor
            font.pointSize: parameters.txtSize
            rotation: parameters.horizontal ? 0:90
            anchors.topMargin: parameters.horizontal ? 5:  0.5 * width
            anchors.leftMargin: parameters.horizontal ? 5: 5 - 0.5 * width
        }

        // Center Value
        Text {
            anchors.top:parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            text: centerBar.toFixed(parameters.nbdigit)
            color: parameters.txtColor
            font.pointSize: parameters.txtSize
            rotation: parameters.horizontal ? 0:90
            anchors.topMargin: parameters.horizontal ? 5: 0.5 * width

        }
        // Max Value
        Text {
            anchors.top:parent.bottom
            anchors.right: parent.right
            text: maxBar.toFixed(parameters.nbdigit)
            color: barValue >= maxBar ? "red" : parameters.txtColor
            font.pointSize: parameters.txtSize
            rotation: parameters.horizontal ? 0:90
            anchors.topMargin: parameters.horizontal ? 5:  0.5 * width
            anchors.rightMargin: parameters.horizontal ? 5: 5 - 0.5 * width
        }
    }
}
