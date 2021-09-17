import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Window 2.0
import QtQuick.Layouts 1.14

Window {
    id:popupinspect
    x: 100;
    y: 100;
    width: 600;
    height: 600;
    visible: true
    property var idnode: ""



    TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Params")
        }
        TabButton {
            text: qsTr("Discover")
        }
        TabButton {
            text: qsTr("Activity")
        }
    }

    StackLayout {
        width: parent.width
        currentIndex: bar.currentIndex
        Item {
            id: homeTab
            Inspector {idnodee:idnode}
        }
        Item {
            id: discoverTab
        }
        Item {
            id: activityTab
        }
    }
}
