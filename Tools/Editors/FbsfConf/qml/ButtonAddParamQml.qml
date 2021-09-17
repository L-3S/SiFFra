import QtQuick 2.0
import QtQuick.Controls 2.4

//Item {
//    id: butaddparam
//    property var modell: []
    ComboBox {
        id: selectparam
        width: 180
//        height: 100
        model: []

        onCurrentIndexChanged: {
            console.debug(model[currentIndex]);
            if (model[currentIndex] !== "")
                addnew(selectparam, model[currentIndex], currentIndex);
        }
        anchors.top:parent.bottom
    }

//}
