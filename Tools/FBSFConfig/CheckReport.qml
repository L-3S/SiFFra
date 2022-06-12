import QtQuick 2.0
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12
Window {
    id: root
    title : "Check report"
    width: 600; height: 250
    property alias report : report.text
    ScrollView{
        anchors.fill : parent
        contentHeight :panel.height
        Rectangle {id:panel
            width: root.width;
            height : report.height<root.height?root.height:report.height
            color : "#5a5a5a"
            Text{id : report
                font.pixelSize: 16
                width: parent.width - 2
                wrapMode: Text.WordWrap
                color:"white"
            }
        }
    }

}

