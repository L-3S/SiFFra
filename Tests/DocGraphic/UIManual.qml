import QtQuick 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import fbsfplugins 1.0

Rectangle {id : root
    objectName: "UIManual"

    anchors.fill : parent
    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#f1bfeb";
        }
        GradientStop {
            position: 0.52;
            color: "#d5eedf";
        }
        GradientStop {
            position: 1.00;
            color: "#9af9f8";
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function statusChanged(mode,state)
    {
        if (state==="running")statusIndicator.color="green"
        else if (state==="paused")statusIndicator.color="red"
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property real mTime: 0
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Column{
        spacing : 40
        x : 40
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Interface with model simulation time
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Row{
            anchors.leftMargin: 10
            spacing : 20
            StatusIndicator {id : statusIndicator
                anchors.topMargin: 10
                active : true
                color:"blue"
            }
            SubscribeInt{id:time;
                tag1: "Data.Time";
                onValueChanged: {mTime = FbsfTimeManager.isUnitMS?value/1000:value;
                    simtime.text=tag1+" : "+mTime.toFixed(3)}
            }

            Text{id:simtime;font.pointSize: 16}
        }
        Text{text:"Time unit :"
                  +(FbsfTimeManager.isUnitMS?"millisecondes":"seconds")
            font.pointSize: 16}
        Text{id:stepCount;text:"Step count : "+FbsfTimeManager.StepCount;font.pointSize: 16}

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Subscription to scalars values
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Row{
            anchors.leftMargin: 10
            SubscribeInt {id:gCounter;
                tag1: "Producer.Counter";
                onValueChanged:{txtInt.text=tag1+"="+ value ; }
            }
            Text{id:txtInt;font.pointSize: 16}
        }

        Row{
            anchors.leftMargin: 10
            SubscribeReal {id:gOnR;
                tag1: "Producer.random_pos";
                onValueChanged:{txtReal.text=tag1+"="+ value ; }
            }
            Text{id:txtReal;font.pointSize: 16}
        }
    }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subscription to vectors values
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeVectorInt{id:vectorInt
        tag1: "Producer";tag2: "VectorInt";
        onDataChanged: {txt1.text="UI[0-4]/"+tag1+"[5-9] : " +data}
    }
    Column{
        spacing : 10
        x:20;y:350
        Text{id:title;text:"Vector of Integers";font.pointSize:16;
            anchors.horizontalCenter:parent.horizontalCenter}

        Row{id: levels
            spacing : 10
            Repeater{
                model : vectorInt.data.length
                delegate :
                    Gauge {
                    x:index*10
                    value: vectorInt.data[index]
                    tickmarkStepSize: 20
                    minorTickmarkCount: 1
                    font.pixelSize: 15

                    style: GaugeStyle {
                        valueBar: Rectangle { color: index<5?"#04eadb":"#178ae2";implicitWidth: 28  }
                        background: Rectangle{anchors.fill : parent;color:"grey";}
                        tickmarkLabel : Text {
                            text: control.formatValue(styleData.value)
                            font: control.font
                            color: "grey"
                            antialiasing: true
                        }
                        tickmark: Item {
                            implicitWidth: 8
                            implicitHeight: 2

                            Rectangle {
                                x: control.tickmarkAlignment === Qt.AlignLeft
                                   || control.tickmarkAlignment === Qt.AlignTop ? parent.implicitWidth : -28
                                width: 28
                                height: parent.height
                                color: "#ffffff"
                            }
                        }

                        minorTickmark: Item {
                            implicitWidth: 8
                            implicitHeight: 1

                            Rectangle {
                                x: control.tickmarkAlignment === Qt.AlignLeft
                                   || control.tickmarkAlignment === Qt.AlignTop ? parent.implicitWidth : -28
                                width: 28
                                height: parent.height
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }
        }
        Text{id:txt1;font.pointSize: 12}
        Button{x:300
            text: "increase [0-4]"
            onClicked: {sigVectorInt.val+=5; }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeVectorReal{id:vectorReal
        tag1: "Producer";tag2: "VectorReal"
        onDataChanged: {txt2.text="UI[0-4]/"+tag1+ "[5-9] : "+data}
    }

    Column{
        spacing : 10
        x:800;y:350
        Text{id:title2;text:"Vector of real";font.pointSize: 16
        anchors.horizontalCenter:parent.horizontalCenter}

        Row{id: levels2
            spacing : 10
            Repeater{
                model : vectorReal.data.length
                delegate :
                    Gauge {
                    x:index*10
                    value: vectorReal.data[index]
                    tickmarkStepSize: 20
                    minorTickmarkCount: 1
                    font.pixelSize: 15

                    style: GaugeStyle {
                        valueBar: Rectangle { color: index<5?"#ffbb7a":"#ee381c";implicitWidth: 28  }
                        background: Rectangle{anchors.fill : parent;color:"grey";}
                        tickmarkLabel : Text {
                            text: control.formatValue(styleData.value)
                            font: control.font
                            color: "grey"
                            antialiasing: true
                        }
                        tickmark: Item {
                            implicitWidth: 8
                            implicitHeight: 2

                            Rectangle {
                                x: control.tickmarkAlignment === Qt.AlignLeft
                                   || control.tickmarkAlignment === Qt.AlignTop ?
                                       parent.implicitWidth : -28
                                width: 28
                                height: parent.height
                                color: "#ffffff"
                            }
                        }

                        minorTickmark: Item {
                            implicitWidth: 8
                            implicitHeight: 1

                            Rectangle {
                                x: control.tickmarkAlignment === Qt.AlignLeft
                                   || control.tickmarkAlignment === Qt.AlignTop ? parent.implicitWidth : -28
                                width: 28
                                height: parent.height
                                color: "#ffffff"
                            }
                        }
                    }
                }
            }
        }
        Text{id:txt2;font.pointSize: 12}
    }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Signaling vector value changes to C++
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SignalVectorInt{id : sigVectorInt;
        tag1: "UIManual.VectorInt";
        property int val:0;
        data: [0+val,1+val,2+val,3+val,4+val]
    }

    SignalVectorReal{id : sigVectorReal
        tag1: "UIManual.VectorReal"
        data:[1+mTime,2+mTime,3+mTime,4+mTime,5+mTime]
    }
}
//    Text {
//        anchors.centerIn: parent
//        font.pointSize: 16
//        text: "<a href='"+FBSF_HOME+"Documentation/html/Plotter.xhtml'>help</a>"
//        onLinkActivated: Qt.openUrlExternally(link)

//        MouseArea {
//            anchors.fill: parent
//            acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
//            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
//        }
//    }
