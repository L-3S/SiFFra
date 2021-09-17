import QtQuick 2.0

DropArea {
    id:dragzone
    x: 100; y: 75
    width: 50; height: 50
    property var pare: ""

    Rectangle {
        id:rect
        anchors.fill: parent
        color: "green"
        property var savmouse: "";


        visible: false;
        Component.onCompleted:windowSimulation.racdrag.push(rect);

        states: [
            State {
                when:
                {
                    if (dragzone.containsDrag && windowSimulation.canplace) {
                        windowSimulation.canplace = false;
                        var newidnode;
                        if (dragzone.pare.list.length === 0) {
                            console.log("add normally");
                            newidnode = spaceManager.addChildModules(idnode, windowSimulation.nbtype);
                            createModel(pare, pare.list, newidnode, "ModuleQml.qml");
                        }
                        else {
                            console.log("add between");
                            newidnode = spaceManager.addChildModulesBetween(idnode, false, windowSimulation.nbtype);
                            dragzone.pare.list = createModelBetween(dragzone.pare, dragzone.pare.list, newidnode, "ModuleQml.qml");
                        }
                    }
                    dragzone.containsDrag
                }
                PropertyChanges {
                    target: rect
                    color: "grey"

                }
            }

        ]
    }
}
