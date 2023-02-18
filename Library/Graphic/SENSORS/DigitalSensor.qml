import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node
{
    /////////////////////////////////////////////////
    // Inspector parameters:
    /////////////////////////////////////////////////
    property var parameters : {"varName":"",
        "unit" :"Bar",
        "unit conv coeff A" : 1.,
        "unit conv coeff B" : 0.,
        "width":100,
        "height":30,
        "ndigit" : 2,
        "textColor" : "black",
        "bgColor": "green",
        "idxShift" : 0
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
    SubscribeReal{id: inputScalar
        // In SimuMPC mode this variable is not needed
        tag1 : simuMpc ? "" : parameters.varName
        onValueChanged: {
            /// including unit conversion y = a * x + b
            var loc = parameters["unit conv coeff A"] * value + parameters["unit conv coeff B"]
            textValue.text = loc.toFixed(parameters.ndigit)
        }
    }
    SubscribeVectorReal {id: inputVector
        // In normal mode this variable is not needed
        tag1 : simuMpc ? parameters.varName  : ""
        onDataChanged: {
            /// including unit conversion y = a * x + b
            // Case simuMpc mode get last value of the past data[pastSize-1]
            // we add the idx shift
            var loc = parameters["unit conv coeff A"] * data[pastSize-1+parameters["idxShift"]] + parameters["unit conv coeff B"]
            textValue.text = loc.toFixed(parameters.ndigit)
        }
    }

    //Control the shape of the Node model
    shape.visible : false
    shape.width : parameters.width
    shape.height : parameters.height

    Rectangle {id: bg
        width: parameters.width
        height: parameters.height
        border.color: "black"
        border.width: 2
        radius: 4
        color: parameters.bgColor;
        Text {id:textValue
            text: "0.00"
            color: parameters.textColor
            anchors {left: parent.left; leftMargin:10; verticalCenter: parent.verticalCenter}
            font.pointSize: 10
        }
        Text {id:textUnit
            text: parameters.unit
            color: parameters.textColor
            anchors {right: parent.right; rightMargin:10; verticalCenter: parent.verticalCenter}
        }
    }
}
