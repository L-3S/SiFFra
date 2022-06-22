import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import Qt.labs.qmlmodels 1.0

Popup {id:popup
    visible : false
    width   : 450
    height  : 260
    property string selected_time:""
    property string isoFormat  : 'hh:mm:ss'  // internal
    property string timeFormat : isoFormat   // external
    property string timeHour    : "00"   // internal
    property string timeMinutes : "00"   // internal
    property string timeSeconds : "00"   // internal
    property string timeSelected: timeHour+":"+timeMinutes+":"+timeSeconds   // internal
    property int hourIndex      : -1    // for highlight
    property int minutesIndex   : -1    // for highlight
    property int mnStep         : 5
    signal timeChanged(string time)

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onSelected_timeChanged: {
        // update selectors
        var hours=selected_time.split(":")[0]
        var minutes=selected_time.split(":")[1]
        hourIndex=parseInt(hours)
        minutesIndex=parseInt(minutes)/mnStep
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Column {
        focus   : true
        width   : popup.width
        height  : popup.height
        spacing : 10
        anchors.horizontalCenter: popup.horizontalCenter
        Text{text:"hours :";color:"white"}
        Grid {id: hoursGrid
            Layout.fillWidth: true
            columns: 12
            rows: 2
            Repeater {

                function zeroPad(number) {
                    return number > 9 ? number : '0' + number;
                }
                model:{
                    var hours = [];
                    for (var i = 0; i < 24; ++i)
                    {
                        hours[i] = zeroPad(i)
                    }
                    return hours;
                }
                delegate: Button {
                    width: 35
                    height: 35
                    highlighted: hourIndex===index
                    text: modelData
                    onClicked: {
                        timeHour=modelData
                        hourIndex=index
                        timeChanged(timeSelected)
                    }
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Rectangle{width : hoursGrid.width;height : 2;color : "silver"}
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Text{text:"minutes :";color:"white"}
        Grid {id: minutesGrid
            width : hoursGrid.width
            columns: 12
            rows: 1

            Repeater {
                model:{
                    var minutes = [];

                    for (var i = 0; i < 12; ++i)
                    {
                        minutes[i] = i*mnStep
                    }
                    return minutes;
                }
                delegate: Button {
                    width: 35
                    height: 35
                    highlighted: minutesIndex===index
                    text: modelData
                    onClicked: {
                        timeMinutes=modelData
                        minutesIndex=index
                        timeChanged(timeSelected)
                    }
                }
            }
        }// minutes
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Rectangle{width : hoursGrid.width;height : 2;color : "silver"}
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // selection of the time format
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Row{
            Text{id:formatText;text:"format :";color:"white"
                anchors.verticalCenter: formatField.verticalCenter
            }

            ComboBox {id:formatField
                width: hoursGrid.width-formatText.width
                editable: true
                selectTextByMouse: true
                model: ["","hh.mm.ss","hhh:mm:ss","mmm:ss","sss"]
                editText:timeFormat
                // field format changed
                onAccepted: {
                    timeFormat=editText
                }
                // item selection
                onActivated: {
                    displayText=currentText
                    timeFormat=currentText
                }
            }
        }//Row
    }//Column
}
