import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

ColumnLayout{
     id                  : rightToolBar
     anchors.bottom      : parent.bottom
     anchors.right       : parent.right
     anchors.bottomMargin : 20
     anchors.rightMargin : 20
     spacing             : 2
     z                   : parent.z+1

     ToolButton {
         icon.color: "transparent"
         background: Rectangle {color:"gray";radius: 5;implicitWidth: rightToolBar.width}
         icon.source: "qrc:/icons/zoomIn.png"
         ToolTip.visible: hovered
         ToolTip.text: qsTr("Zoom in")
         onClicked:{if(zoomFactor<zoomFactorMax) zoomFactor*=1.1}
     }
     ToolButton {
         icon.color: "transparent"
         background: Rectangle {color:"gray";radius: 5;implicitWidth: rightToolBar.width}
         icon.source : "qrc:/icons/zoomOut.png"
         ToolTip.visible: hovered
         ToolTip.text: qsTr("Zoom out")
         onClicked   : {if(zoomFactor>zoomFactorMin) zoomFactor*=0.9}
     }
     ToolButton {
         icon.color: "transparent"
         background: Rectangle {color:"gray";radius: 5}
         icon.source : "qrc:/icons/zoomReset.png"
         ToolTip.visible: hovered
         ToolTip.text: qsTr("Reset zoom")
         onClicked   : {
             zoomFactor=1;
             //configArea1.returnToBounds()
             //configArea1.contentX=0;configArea1.contentY=0
         }
     }
 }

