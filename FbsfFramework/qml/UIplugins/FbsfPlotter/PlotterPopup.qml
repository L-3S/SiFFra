import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1

import fbsfplugins 1.0
import "qrc:/qml/Components"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Popup window with x plot (tooltip, zoom with ghost rectangle and mouse wheel)
// options for chart settings
// usage :  call display(variables names) on click (return the component ref)
//
//          call popup.positionView(pos) to move the X origin
//          call popup.shiftView(delta) with delta <0 left;delta >0 right
//          call popup.update() to update and scroll the chart
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    property var popup
//
//    PlotterPopup{id:pw}
//
//    Rectangle
//    {
//        x:400;y:300;width:100;height:50;color:"lightgreen"
//
//        MouseArea{anchors.fill:parent
//            onClicked: {popup= pw.display(["Producer.VectorReal_0",
//                                           "Producer.VectorReal_1",
//                                           "Producer.VectorReal_2"]);}
//        }
//    }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Window {
    id: popupWindow
    title: ""
    width: 800
    height: 630
    visible : false

    property bool isPopup   : true
    property bool isStandard   : false

    property var options :{"showTooltips"   : true,
                           "pointDot"       : false,
                           "datasetFill"    : true,
                           "steppedLine"    : false,
                           "bezierCurve"    : true,
                           "xmarker"        : true,
                           "scaleShowLabels": true,
                           "scaleFontColor" : "black",
                           "scaleFontSize"  : 14
    }
    property var    currentPlotArea  : plotArea
    property var    timeBase                            // time of data
    property int    stdViewSize     : 200               // standard mode
    property int    fullSize        : 0                 // data history size
    property int    futurSize       : 0                 // time depend mode (project setting)
    property int    viewStart       : 0                 // X origin point
    property bool   flagUpdateView  : false             // update flag always false

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeInt        { id : timeBaseSTD ; tag1 : simuMpc ? "" : "Data.Time"}
    SubscribeVectorInt  { id : timeBaseMPC ; tag1 : simuMpc ? "Data.Time" : ""}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted:
    {
        // switch on the relevant time base according the mode
        isStandard=!simuMpc
        timeBase=isStandard?timeBaseSTD:timeBaseMPC
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function display(names)
    {
        var component = Qt.createComponent("PlotterPopup.qml");
        if (component.status === Component.Ready)
        {
            var popup = component.createObject(parent,{title:"Popup plotter",
                                               viewSize:timeBase.data.length
                                           });
            if (popup === null) console.log("Error creating object")
            else popup.show();
            popup.registerPlots( names)
        }
        else if (component.status === Component.Error) {
            console.log("Error loading component:", component.errorString());
        }
        return popup
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PlotterArea {id:plotArea;anchors.fill:parent}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function registerPlots(names)
    {
        plotArea.plotUnitNumber=0
        for (var i=0;i < names.length;i++)
        {
            plotArea.setVisible(i,true)
            plotArea.selectedPlot.registerPlot(FbsfDataModel.modelIndex(names[i]))
            plotArea.selectedPlot.options=options
        }
        visible=true
        futurSize= isStandard ? 0:timeBase.data.length-timeBase.timeindex
        fullSize = timeBase.history.length+futurSize

        currentPlotArea.viewSize=isStandard ? stdViewSize : timeBase.data.length

        plotArea.updateData()
        plotArea.updateView(1)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to set the position of the plotter window
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function shiftView(pos)
    {
        plotArea.shiftView(pos)
    }    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to set the position of the plotter window
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function positionView(pos)
    {
        plotArea.positionView(pos)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to update the plotter window
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function update()
    {
        fullSize = timeBase.history.length+futurSize
        currentPlotArea.viewSize=isStandard ? stdViewSize : timeBase.data.length

        plotArea.updateData()
        plotArea.updateView(1)
    }

    property var    chk1 //dummy var
    property var    chk2 //dummy var
    property var    chk3 //dummy var
    property var    chk4 //dummy var
}
