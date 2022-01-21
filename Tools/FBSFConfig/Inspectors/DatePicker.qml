import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import Qt.labs.qmlmodels 1.0


Popup {id:popup
    visible : false
    property date selected_date: new Date()
    property string isoFormat  : 'yyyy-MM-dd'// internal
    property string dateFormat : isoFormat   // external
    property bool startMonDay  : true // 0 Monday , 1 Sunday

    width: 300
    height: 400
    property date _temp_date: selected_date
    Component.onCompleted: sync_calendar(selected_date)
    signal dateChanged(string date)

    function days_in_a_month(y, m) {
        return new Date(y, m + 1, 0).getDate()
    }

    function week_start(y, m) {
        var firstDay=new Date(y, m, 1).getDay()
        return startMonDay?(firstDay===0? 6:firstDay-1):firstDay
    }

    function sync_calendar(day) {
        const month = day.getMonth()
        const year = day.getFullYear()
        const total_days = days_in_a_month(year, month)
        const first_day = week_start(year, month)

        model.clear()
        for (let filler = 0; filler < first_day; filler++) {
            model.append({ day: 0 })
        }

        for (let i = 1; i <= total_days; i++) {
            model.append({ day: i })
        }
    }

    function next_month() {
        const _date = _temp_date
        _date.setMonth(_temp_date.getMonth() + 1)
        sync_calendar(_date)
        return _date
    }

    function previous_month() {
        const _date = _temp_date
        _date.setMonth(_temp_date.getMonth() - 1)
        sync_calendar(_date)
        return _date
    }
    function next_year() {
        const _date = _temp_date
        _date.setFullYear(_temp_date.getFullYear() + 1)
        sync_calendar(_date)
        return _date
    }

    function previous_year() {
        const _date = _temp_date
        _date.setFullYear(_temp_date.getFullYear() - 1)
        sync_calendar(_date)
        return _date
    }
    Column {
        focus   : true
        //width   : popup.width
        height  : popup.height
        anchors.horizontalCenter: popup.horizontalCenter
        // display date with selected format
        TextField {
            id: date_field
            text: selected_date.toLocaleDateString(Qt.locale(), dateFormat)
            width: parent.width
            readOnly: true
            horizontalAlignment: TextInput.AlignHCenter
        }

        ColumnLayout {
            height  : 340
            //width   : popup.width

            RowLayout {
                RoundButton {
                    text: '<<'
                    onClicked: _temp_date = previous_year()
                }
                RoundButton {
                    text: '<'
                    onClicked: _temp_date = previous_month()
                }
                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: _temp_date.toLocaleDateString(Qt.locale(), 'MMM yyyy')
                    font.pixelSize: 18
                }
                RoundButton {
                    Layout.alignment: Qt.AlignRight
                    text: '>'
                    onClicked: _temp_date = next_month()
                }
                RoundButton {
                    Layout.alignment: Qt.AlignRight
                    text: '>>'
                    onClicked: _temp_date = next_year()
                }
            }

            Grid {
                Layout.fillWidth: true
                Layout.fillHeight: true
                columns: 7
                rows: 7
                leftPadding: 10

                Repeater {
                    model:{
                        var weekDays = [];

                        var curDate = new Date();
                        for (var i = 0; i < 7; ++i)
                        {
                            var day=curDate.getDay()
                            day=startMonDay?(day===0?6:day-1):day
                            weekDays[day] = curDate.toLocaleDateString(Qt.locale(), 'ddd');
                            curDate.setDate(curDate.getDate() + 1);
                        }
                        return weekDays;
                    }
                    delegate: Label {
                        width: 35
                        height: 35
                        horizontalAlignment: Text.AlignHCenter
                        text: modelData
                    }
                }

                Repeater {
                    model: ListModel { id: model }

                    delegate: DelegateChooser {
                        DelegateChoice {
                            roleValue: 0

                            delegate: Item {
                                width: 35
                                height: 35
                            }
                        }

                        DelegateChoice {
                            delegate: Button {
                                width: 35
                                height: 35
                                text: day
                                highlighted: day === _temp_date.getDate()
                                             && selected_date.getMonth() === _temp_date.getMonth()
                                             && selected_date.getFullYear() === _temp_date.getFullYear()
                                onClicked: {
                                    const _date = _temp_date
                                    _date.setDate(day)
                                    _temp_date = _date
                                    selected_date = _temp_date
                                    // CHANGE : ISO 8601 format
                                    dateChanged(selected_date.toLocaleDateString(Qt.locale(), isoFormat))
                                }
                            }
                        }
                    }
                }
            }
            // selection of the date format
            RowLayout{
                Text{id:formatText;text:"format :";color:"white"}
                ComboBox {id:formatField
                    Layout.fillWidth: true
                    editable: true
                    selectTextByMouse: true
                    model: ["","dd-MM-yyyy","dd/MM/yyyy","dd.MM.yyyy","ddd, d-MMM-yyyy"]
                    editText:dateFormat
                    // field format changed
                    onAccepted: {
                        dateFormat=editText
                    }
                    // item selection
                    onActivated: {
                        displayText=currentText
                        dateFormat=currentText
                    }
                }
            }
        }
    }
}
