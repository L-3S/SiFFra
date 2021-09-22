import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

PartialMI
{
    nodeType: "MaxMin"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set output socket
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // TO UPDATE SOCKET IN AN INHERITED MODEL
    socketsTop: [
        Socket {id: max;socketId:"max";direction: "output";type: "real"}
                ]
    socketsBottom: [
        Socket {id: min;socketId:"min";direction: "output";type: "real"}
                ]

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        max.value = socketsLeft[0].value;
        min.value = socketsLeft[0].value;
        for (var i=1;i< socketsLeft.length;i++)
        {
                if ( max.value < socketsLeft[i].value)
                    max.value =socketsLeft[i].value;
                if ( socketsLeft[i].value < min.value)
                    min.value =socketsLeft[i].value;
        }
    }
}

