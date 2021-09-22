import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node
{
    source : status ? "BinSensorOn.png":"BinSensorOff.png"
    property bool status : false

    /////////////////////////////////////////////////
    // Inspector parameters:
    /////////////////////////////////////////////////
    property var parameters : {"varName":""}

    /////////////////////////////////////////////////
    // Update Parameters
    /////////////////////////////////////////////////
    Component.onCompleted:{
        parametersChanged()
    }

    /////////////////////////////////////////////////
    // Subscribes in standart or SimuMPC mode
    /////////////////////////////////////////////////
    SubscribeInt{id:inputScalar
        // In SimuMPC mode this variable is not needed
        tag1 : simuMpc ? "" : parameters.varName
        onValueChanged: {
            status = (value === 0 ? false:true)
        }
    }

    SubscribeVectorInt {id: inputVector
        // In normal mode this variable is not needed
        tag1 : simuMpc ? parameters.varName  : ""
        onDataChanged: {
            // Case simuMpc mode get last value of the past data[pastSize-1]
            status = (data[pastSize-1] === 0 ? false:true)
        }
    }
}

