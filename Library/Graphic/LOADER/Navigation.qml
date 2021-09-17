import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node
{
    source : parameters.Up ? "NavigationUp.png":"NavigationDown.png"
    textName.visible : false

    /////////////////////////////////////////////////
    // Inspector parameters:
    /////////////////////////////////////////////////
    property var parameters : {"VisuName":"Plotter","Up":true,"title": "Navigation Title"}


    /////////////////////////////////////////////////
    // Update Parameters
    /////////////////////////////////////////////////
    Component.onCompleted:{
        parametersChanged()
    }

    // Title
    Text {
        text: parameters.title
        anchors.bottom: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 5
        font.pixelSize: 12
        font.bold: true
    }
    // Click Zone to navigate
    MouseArea{
        anchors.fill : parent
        onClicked:{
            displayVisu(parameters.VisuName)
        }
    }
}
