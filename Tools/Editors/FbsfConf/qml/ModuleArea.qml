import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12

import QtQuick.Controls.Styles 1.4

    Flickable {
        id:flick
        width: parent.width
        height: parent.height

        maximumFlickVelocity: 1500
        property var heightcontent: height
        contentWidth: width; contentHeight: heightcontent
        contentY: 0
        SimuModelQml {}
    }
