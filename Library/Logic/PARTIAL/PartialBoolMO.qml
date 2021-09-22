import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

//Node
PARTIALGENERIC
{
    nodeType: "PartialBoolMO"
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set output socket
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // TO UPDATE SOCKET IN AN INHERITED MODEL
    property alias output : y
    socketsRight: [
        Socket {id: y;socketId:"y";direction: "output";type: "bool"}
                ]
}

