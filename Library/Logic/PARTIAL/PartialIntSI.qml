import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0

//Node
PartialGeneric
{
    nodeType: "PartialIntSI"
    gradiant1: "lightgreen"
    gradiant2: gradiant1
    shape.width: 100
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set input socket
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // TO UPDATE SOCKET IN AN INHERITED MODEL
    property alias input : u
    socketsLeft: [
        Socket {id: u;socketId:"u";direction: "input";type: "int"}
                ]

}

