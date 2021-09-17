import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

ColumnLayout{
     id                  : rightToolBar
     anchors.top         : parent.top
     anchors.right       : parent.right
     anchors.topMargin   : 20
     anchors.bottomMargin: 5
     anchors.rightMargin : 10
     spacing             : 2
     z                   : parent.z+1

     ToolButton {
         icon.color: "transparent"
         background: Rectangle {color:"gray";radius: 5;implicitWidth: rightToolBar.width}
         icon.source: "qrc:/icons/zoomIn.png"
         ToolTip.visible: hovered
         ToolTip.text: qsTr("Zoom in")
         onClicked:{zoom*=1.1}
     }
     ToolButton {
         icon.color: "transparent"
         background: Rectangle {color:"gray";radius: 5;implicitWidth: rightToolBar.width}
         icon.source : "qrc:/icons/zoomOut.png"
         ToolTip.visible: hovered
         ToolTip.text: qsTr("Zoom out")
         onClicked   : {if(zoom>0.5) zoom*=0.9}
     }
     ToolButton {
         icon.color: "transparent"
         background: Rectangle {color:"gray";radius: 5}
         icon.source : "qrc:/icons/zoomReset.png"
         ToolTip.visible: hovered
         ToolTip.text: qsTr("Reset zoom")
         onClicked   : {
             zoom=1;
             //configArea1.returnToBounds()
             //configArea1.contentX=0;configArea1.contentY=0
         }
     }
 }

