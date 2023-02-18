import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node
{
    /// subscribe to a real value
    /// compares this value to a threshold
    /// status = (in > threshold)
    /// green display if status === true, else red


    source : status ? "BinSensorOn.png":"BinSensorOff.png"
    property bool status : false

    /////////////////////////////////////////////////
    // Inspector parameters:
    /////////////////////////////////////////////////
    property var parameters : {
        "varName":"",
        "threshold" :0.5
    }

    /////////////////////////////////////////////////
    // Update Parameters
    /////////////////////////////////////////////////
    Component.onCompleted:{
        parametersChanged()
    }

    /////////////////////////////////////////////////
    // Subscribes in standart or SimuMPC mode
    /////////////////////////////////////////////////
    SubscribeReal{id:inputScalar
        // In SimuMPC mode this variable is not needed
        tag1 : simuMpc ? "" : parameters.varName
        onValueChanged: {
            status = ((data[pastSize-1] > parameters.threshold) ? true:false)
        }
    }

    SubscribeVectorReal {id: inputVector
        // In normal mode this variable is not needed
        tag1 : simuMpc ? parameters.varName  : ""
        onDataChanged: {
            // Case simuMpc mode get last value of the past data[pastSize-1]
            status = ( (data[pastSize-1] > parameters.threshold) ? true:false)
        }
    }
}

