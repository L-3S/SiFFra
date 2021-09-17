import QtQuick 2.0
import QtQuick.Controls 2.14

Button {
    id: butsav
//    y: 50
    property var savfunc: ""
        text: "Save"
        onClicked: {
//            var newidnode = spaceManager.addChildModules(idnode);
//            createModel(pare, pare.list, newidnode, "ModuleQml.qml");
            savfunc();
        }
//        anchors.top:parent.bottom + 200

}
