import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node {
    /// Displays the date and time (local settings)
    /// using a sechsFromEpoch "int" in the exch zone (Data.Time)


    /////////////////////////////////////////////////
    // Inspector parameters:
    /////////////////////////////////////////////////
    property var parameters : {
        "varName"       : "Data.Time"   ,
        "disp_year"     : true          ,
        "disp_month"    : true          ,
        "disp_day"      : true          ,
        "disp_hr"       : false         ,
        "disp_min"      : false         ,
        "disp_sec"      : false         ,
        "width"         : 100           ,
        "height"        : 30            ,
        "textColor"     : "black"       ,
        "bgColor"       : "green"       ,
        "idxShift"      : 0
    }

    property var locale: Qt.locale()

    function retDate(secs_since_epoch) {
        var date_str = FbsfDataModel.getDateFromTime (secs_since_epoch)
        return Date.fromLocaleString(locale, date_str, "yyyy MM dd HH:mm:ss") // Format of Date "yyyy MM dd HH:mm:ss"
    }

    function retString(date) {
        var year    = (parameters.disp_year ) ? ' ' +   date.toLocaleString(locale, "yyyy") : '';
        var month   = (parameters.disp_month) ? ' ' +   date.toLocaleString(locale, "MMM") : '';
        var daySTR  = (parameters.disp_day  ) ?         date.toLocaleString(locale, "ddd") : '';
        var dayNB   = (parameters.disp_day  ) ? ' ' +   date.toLocaleString(locale, "dd") : '';
        var hr      = (parameters.disp_hr   ) ? ' ' +   date.toLocaleString(locale, "HH") : '';
        var min     = (parameters.disp_min  ) ? ' :' +  date.toLocaleString(locale, "mm") : '';
        var sec     = (parameters.disp_sec  ) ? ' :' +  date.toLocaleString(locale, "ss") : '';


        return daySTR + dayNB + month + year + hr + min + sec;
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
    SubscribeInt{id: inputScalar
        // In SimuMPC mode this variable is not needed
        tag1 : simuMpc ? "" : parameters.varName
        onValueChanged: {
            var secs_since_epoch = value

            textValue.text = retString(retDate(secs_since_epoch))
        }
    }
    SubscribeVectorInt {id: inputVector
        // In normal mode this variable is not needed
        tag1 : simuMpc ? parameters.varName  : ""
        onDataChanged: {
            // Case simuMpc mode get last value of the past data[pastSize-1]
            // we add the idx shift
            var secs_since_epoch = data[pastSize-1+parameters["idxShift"]]

            textValue.text = retString(retDate(secs_since_epoch))
        }
    }

    //Control the shape of the Node model
    shape.visible : false
    shape.width : parameters.width
    shape.height : parameters.height

    Rectangle {
        id: bg
        width: parameters.width
        height: parameters.height
        border.color: "black"
        border.width: 2
        radius: 4
        color: parameters.bgColor;
        Text {id:textValue
            text: "2019"
            color: parameters.textColor
            anchors {horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter}
            font.pointSize: 10
        }
    }
}
