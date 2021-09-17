import QtQuick 2.0
import QtQuick.Controls 1.4

Item {
    id: configc
//    width: 20
    height: 60
    property var textt: ""
    property var title: ""
    property var test: ""
    property var limite: ""
    property var type: "int"
    property int min: 0
    property int max: 0

    Text {
        id: configcasetitle
        text: "(int) " + limite +" "+ title + ":"
        font.pixelSize: 18
    }

    TextField {
        id: configcase
        text: textt
        font.pixelSize: 18
        anchors.top:configcasetitle.bottom
        validator: RegExpValidator{regExp: /-?[0-9]+/}
        Component.onCompleted:configc.test = configcase;
    }

//    anchors.top:parent.bottom
}
