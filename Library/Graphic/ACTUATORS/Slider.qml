import QtQuick 2.0
import QtQuick.Controls 2.4
import Grapher 1.0
import fbsfplugins 1.0

Node
{
    property var parameters : {"varName":"varName", "unit" :"u", "description" : "d",
                               "min" : 0. ,"max" : 100.}
    Component.onCompleted: {
        output.tag1 = parameters.varName
        output.unit = parameters.unit
        output.description = parameters.description

        sld.from = parameters.min
        sld.to = parameters.max
        sld.value = 0.5 * (parameters.max -parameters.min )

    }

    onParameterModified : {
        sld.from = parameters.min
        sld.to = parameters.max
    }

    SignalReal{
        id:output
        value: sld.value
        unit : parameters.unit
        description : parameters.description
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    shape.visible : false
    shape.width : sld.width
    shape.height : sld.height

    Slider {id:sld
        // Case Time Depends no action form actuators alowed
        enabled: ! SimuMpc 
        width:100
    }
}

