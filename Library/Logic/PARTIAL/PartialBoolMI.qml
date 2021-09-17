import QtQuick 2.0
import Grapher 1.0
import QtQuick.Controls 1.1

PartialGeneric
{
    nodeType: "PartialBoolMI"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // build an input sockets array dynamically
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var ports :{"left" :2}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Add sockets dynamically
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted: {
        for(var i=0;i<ports.left;i++)
            socketContainerLeft.setPort("u","input","bool");
    }
}

