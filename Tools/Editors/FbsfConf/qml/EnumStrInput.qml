import QtQuick 2.0
import QtQuick.Controls 1.4

Item {
    id: configc
//    width: 20
    height: 60
    property var textt: 0
    property var title: ""
    property var test: ""
    property var listenu: []

    Text {
        id: configcasetitle
        text: title + ":"
        font.pixelSize: 18
    }

    ComboBox {
        id: configcase
        model: listenu
        anchors.top:configcasetitle.bottom
//        Component.onCompleted:configc.test = model;
        currentIndex: textt
        property var text: listenu[currentIndex]
        onActivated: configcase.text = listenu[currentIndex]
        Component.onCompleted: configc.test = configcase
    }

//    TextField {
//        id: configcase
//        text: textt
//        font.pixelSize: 18
//        anchors.top:configcasetitle.bottom
//        Component.onCompleted:configc.test = configcase;
////            validator: RegExpValidator{regExp: /\d+/}
//    }

//    anchors.top:parent.bottom
}
