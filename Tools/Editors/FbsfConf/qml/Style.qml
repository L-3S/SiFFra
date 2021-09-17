import QtQuick 2.0

Item {
    id: style

    // All Module
    property var radiusmodule: 20
    property var border: 2
    property var widthModule: 120
    property var heigthModule: 120

    // Module
    property var colorModule: "#6e86b5"

    // Node
    property var colorNode: "purple"

    //EndNode
    property var colorEndNode: "maroon"

    //Sequence
    property var colorSequence: "orange"

    //Machine
    property var colorMachine: "green"

    //Simulation
    property var colorSimulation: "red"

    //Background
    property var backgroundcolor: "#b5bbc7"

    //TopMenu
    property var topMenuColor: "grey"

    Component.onCompleted:windowSimulation.style = style;
}
