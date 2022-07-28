import QtQuick 2.2
import QtQuick.Extras 1.4
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import fbsfplugins 1.0

Rectangle {id : root
    objectName: "UIManualMPC"
    property bool  backtrackable:true
    property int pastsize
    property int futursize
    Component.onCompleted:
	{
		pastsize=FbsfTimeManager.PastSize
		futursize=FbsfTimeManager.FuturSize
	}
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
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function statusChanged(mode,state)
    {
        if (state==="running") statusIndicator.color="green"
        else if (state==="paused") statusIndicator.color="red"
    }
	 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     // Subscription to scalars values
     // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeInt {
		id:gCounter;
        tag1: "Producer.Counter";
        onValueChanged:{txtInt.text=tag1+"="+ value
        }
    }
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subscription to Data.Time vector values
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeVectorInt{
		id: dataTimeVector
        tag1 : "Data.Time"
        onDataChanged  : {
            simtime.text=tag1+" : "
								+FbsfTimeManager.dateStr(data[pastsize-1])
								+ "  "
								+FbsfTimeManager.timeStr(data[pastsize-1])
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

            Text{id:simtime;font.pointSize: 16}
        }
		//~~~~~~ Display Scalar value ~~~~~~~
        Row{
            anchors.leftMargin: 10
            Text{id:txtInt;font.pointSize: 16}
        }

    }
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subscription to MPC vector values
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeVectorInt{id:vectorInt
        tag1: "Producer";tag2: "VectorInt_0";
    }
    Column{
        spacing : 10
        x:20;y:300
        Text{id:title;text:"MPC Sizes :"
                           +pastsize
                           +"/"
						   +futursize
            font.pointSize:16;
            anchors.horizontalCenter:parent.horizontalCenter}
        Row{
            spacing : 10
            Repeater{
                model:vectorInt.data.length
                Text{id:txt1
                    text:vectorInt.data[index]
                    color : (index===(pastsize-1)?"red"
												:index<(pastsize-1)?"black":"blue")
                    font.pointSize: 12}
            }
        }
    }

}

