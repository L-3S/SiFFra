import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Controls.Styles 1.4

ScrollView {
    id:viewlist
    Rectangle {
        width: 200
        height: windowSimulation.height
    }

    property var list: createTypeMenu(parent);
    Component.onCompleted: windowSimulation.racList = viewlist;


    function createTypeMenu(parent) {
        var component;
        var savcomponent;

        component = Qt.createComponent("ListType.qml");
        savcomponent = component.createObject(parent, {});
        return savcomponent;
    }

    function reload()
    {
        list.destroy();
        list = createTypeMenu(parent);
        windowSimulation.racList = viewlist;
    }
}
