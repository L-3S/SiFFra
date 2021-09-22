import QtQuick 2.0
import Grapher 1.0
import QtQuick.Controls 1.1

PartialGeneric
{
    nodeType: "PartialSwitchMulti"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // build an input sockets array dynamically
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var ports :{"left" :2}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Add sockets dynamically
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted: {
        for(var i=0;i<ports.left;i++)
            socketContainerLeft.setPort("u","input","real");
    }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set top input socket
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // TO UPDATE SOCKET IN AN INHERITED MODEL
    property alias input : uint
    socketsTop: [
        Socket {id: uint;socketId:"uint";direction: "input";type: "int"}
                ]

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set output socket
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // TO UPDATE SOCKET IN AN INHERITED MODEL
    property alias output : y
    socketsRight: [
        Socket {id: y;socketId:"y";direction: "output";type: "real"}
                ]

}

