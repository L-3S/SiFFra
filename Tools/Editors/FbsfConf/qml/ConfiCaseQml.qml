import QtQuick 2.0
import QtQuick.Controls 1.4

Item {
    id: configc
//    width: 20
    height: 60
    property var textt: ""
    property var title: ""
    property var test: ""

    Text {
        id: configcasetitle
        text: title + ":"
        font.pixelSize: 18
    }

    TextField {
        id: configcase
        text: textt
        font.pixelSize: 18
        anchors.top:configcasetitle.bottom
        Component.onCompleted:configc.test = configcase;
//            validator: RegExpValidator{regExp: /\d+/}
    }

//    anchors.top:parent.bottom
}

