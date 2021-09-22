import QtQuick 2.0
import Grapher 1.0
import QtQuick.Controls 1.1

PartialGeneric
{
    nodeType: "PartialMIMO"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // build an input sockets array dynamically
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var ports :{"leftright":4}
       // property var ports :{"leftright":2,"topbottom":3}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Add sockets dynamically
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted: {
        for(var i=0;i<ports.leftright;i++)
        {
            socketContainerLeft.setPort("I","input","real");
            socketContainerRight.setPort("O","output","real");
        }
    }
}
