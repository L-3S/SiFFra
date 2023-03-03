import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node
{
    /// subscribe to a real value
    /// compares this value to a threshold
    /// status = (in > threshold)
    /// green display if status === true, else red

    property bool status : false

    /////////////////////////////////////////////////
    // Inspector parameters:
    /////////////////////////////////////////////////
    property var parameters : {
        "varName":"",
        "threshold" :0.5,
        "width":100,
        "height":20,
        "text":""
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
            status = (value > parameters.threshold ? true:false)
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

//    SubscribeReal{id:inputReal
//        // In SimuMPC mode this variable is not needed
//        tag1 : simuMpc ? "" : parameters.varName
//        onValueChanged: {
//            status = (value > parameters.threshold ? true:false)
//        }
//    }

//    SubscribeVectorReal {id: inputRealVector
//        // In normal mode this variable is not needed
//        tag1 : simuMpc ? parameters.varName  : ""
//        onDataChanged: {
//            // Case simuMpc mode get last value of the past data[pastSize-1]
//            status = ( (data[pastSize-1] > parameters.threshold) ? true:false)
//        }
//    }

    //Control the shape of the Node model
    shape.visible : false
    shape.width : parameters.width
    shape.height : parameters.height

    Rectangle {id: bg
        visible:!status
        color:"transparent"
        width: parameters.width
        height: parameters.height
        radius: 4;
        Text {id:textValue
            text: parameters.text
            anchors {left: parent.left; leftMargin:10; verticalCenter: parent.verticalCenter}
            font.pointSize: 10
        }
    }
}

