import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import "qrc:/qml/Components"
import FbsfUtilities 1.0

import "qrc:/qml/Javascript/Common.js" as Util

Rectangle {

    // Global plotter properties
    property bool   selected    :false
    property alias  plotter     : plotter

    property var options :{"showTooltips"   : true,
                           "pointDot"       : false,
                           "datasetFill"    : false,
                           "steppedLine"    : false,
                           "bezierCurve"    : false,
                           "xmarker"        : true,
                           "xTimeLine"      : true,
                           "scaleShowLabels": true,
                           "scaleFontColor" : "white",
                           "scaleFontSize"  : 14,
    }
    // Graphic behaviors
    border.width: selected ? 6:1
    border.color: selected ? "lime":"black"
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Register a new plot with data name
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function registerPlot(modelIndex)
    {
        var name = FbsfDataModel.name(modelIndex)
        var unit = FbsfDataModel.unit(modelIndex)
        var plotIndex = getFreeIndex()
        if (plotIndex >= 0)
        {
            var plotData=plotter.chartData.datasets[plotIndex]
            plotData.label=name
            plotData.unit=unit
            plotData.id=modelIndex
            // set display unit
            if (displayUnitMode) displayUnit()
            return plotIndex
        }
        return undefined
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // display unit if only one plot or plots with same unit
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function displayUnit()
    {
        var display=true
        var unit=undefined

        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id !== -1)
            {
                if(unit===undefined) unit=plotData.unit
                else
                if(plotData.unit!==unit)display=false
            }
        }
        if(display)
        {
            plotter.chartData.datasets.displayUnit=displayUnitMap[unit]
        }
        else // reset settings
        {
            plotter.chartData.datasets.displayUnit=undefined
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Register a new plot loaded from csv file
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function registerCSVPlot(name,timeset,dataset)
    {
        var plotIndex = getFreeIndex()
        if (plotIndex < 0) return

        var plotData=plotter.chartData.datasets[plotIndex]
        plotData.label=name+"#"
        plotData.unit=""
        plotData.id=-2
        plotData.set=dataset
        for (var i=0;i< timeset.length;i++)
        {
            var myDate = Util.getDateStr(timeset[i])
            dataSetTime[i]=myDate/1000
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get all model indexes
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getModelIndexes()
    {
        var modelIndexes=[]
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id!==-1)
            {
                modelIndexes.push(plotData.id)
            }
        }
        return modelIndexes
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // get all colors
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getColors()
    {
        var colors=[]
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id!==-1)
            {
                colors.push(plotData.strokeColor)
            }
        }
        return colors
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // get plot colors
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getColor(plotIndex)
    {
        if (plotIndex >= 0)
        {
            var plotData=plotter.chartData.datasets[plotIndex]
            return plotData.strokeColor
        }
        return undefined
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getNbMaxPlot()
    {
         return plotter.chartData.datasets.length
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Remove plot
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function clear(modelIndexes)
    {
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id!==-1)
            {
                modelIndexes.push(plotData.id)
                unsetDataSet(plotData.id)
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function repaint()
    {
         plotter.repaint()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setXTimeLine(value)
    {
         plotter.chartData.xTimeLine=value
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Save active plots values as csv file
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // File IO C++ utility for plotters serialization/deserialization
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FbsfFileIO {
        id: csvFile
        source: "Plotter.csv"
        onError: Util.messageBox("ERROR",msg)
    }
    function saveCsv(filename)
    {
        if (csvFile.open(filename)===false) return
        //~~~~~~~~~~ write plot names in the first line ~~~~~~~~~~~~~~~~~~~~~~~
        var names="time"
        for (var iname=0;iname < plotter.chartData.datasets.length;iname++)
        {
            var plotData=plotter.chartData.datasets[iname]
            if (plotData.id!==-1) names+= ";" + plotData.label
        }
        csvFile.write(names)
        //~~~~~~~~~~ write plot data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (var t=0; t < dataSetTime.length;t++)
        {
            var data=""
            for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
            {
                var plotData=plotter.chartData.datasets[iPlot]
                if (plotData.id!==-1) data+= ";" + plotData.set[t]
            }
            var str=Util.getDateStr(dataSetTime[t])+data

           csvFile.write(str)
        }
        csvFile.close()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function loadCsv(filename)
    {
        //console.log("Loading Csv file",filename)
        csvFile.openRead(filename)
        var line=csvFile.readline();
        //get the variable name(s)
        var VarList=line.split(';')
        var ListArray=[]
        for (var iVar=0;iVar < VarList.length;iVar++)ListArray[iVar]=[]
        var iLine=0;
        while((line=csvFile.readline())!=="")
        {            
            var ValueList = line.split(';');
            for (var iCol=0;iCol < ValueList.length;iCol++)
                ListArray[iCol][iLine] = ValueList[iCol]
            iLine++
        }
        csvFile.close()
        for (var v=1;v < VarList.length;v++)
            registerCSVPlot(VarList[v],ListArray[0],ListArray[v])
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get plot index from model index
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getIndex(modelIndex)
    {
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id===modelIndex)return iPlot
        }
        return -1;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check for free index plotdata.id=-1
    // Special case if -2 : means csv plot
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getFreeIndex()
    {
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id===-1)return iPlot
        }
        console.log("[WARNING]",qsTr("getFreeIndex : no free index"))
        return -1;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get plot data by index
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getPlotData(plotIndex)
    {
        return plotter.chartData.datasets[plotIndex]
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Push new data to set
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setDataSet(modelIndex,dataset)
    {
        var plotIndex=getIndex(modelIndex)
        if (plotIndex >= 0)
        {
            var plotData=plotter.chartData.datasets[plotIndex]

            if (FbsfDataModel.isScalar(modelIndex) && !isStandard)
            {   // case of time depend mode with scalars
                var set=[]
                set.length=timeBase.history.length
                var start=timeBase.history.length-dataset.length

                for (var j=0;j<dataset.length;j++)
                {
                    if (j%timeBase.timeshift==0 )
                        {set[start+j]=dataset[j]}
                    else
                        set[start+j]=set[start+j-1]
                }
                plotData.set=set
            }
            else
            {
                plotData.set=dataset
            }
        }
        return plotIndex
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Reset plot unit data structures
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function unsetDataSet(modelIndex)
    {
        var plotIndex=getIndex(modelIndex)
        if (plotIndex >= 0)
        {
            var plotData=plotter.chartData.datasets[plotIndex]
            plotData.label=""
            plotData.id=-1
            plotData.set=[]
            plotData.data=[]
            displayUnit(plotData)
            plotter.repaint()
        }
        return plotIndex
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Update plot unit dataset
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateData()
    {
        var once=true
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id >= 0)
            {
                if (isStandard)
                {   //~~~~~~~~~~~~~~~~~~~~~~~~ standard mode ~~~~~~~~~~~~~~~~~~~~~~~~
                    if (FbsfDataModel.history(plotData.id).length===0)
                        plotData.set.push(FbsfDataModel.value(plotData.id))// no history
                    else
                        setDataSet(plotData.id,FbsfDataModel.history(plotData.id))
                }
                else //~~~~~~~~~~~~~~~~~~~~~~~~ time depend mode ~~~~~~~~~~~~~~~~~~~~~~~~
                {
                    // set the history dataset and add the futur data to dataset
                    if(once) {once=false;plotter.chartData.xmarker=xmarker}
                    setDataSet(plotData.id,FbsfDataModel.historywithfutur(plotData.id))
                }
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // start : start point for view (xAxisOrigin)
    // length : size of view
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateView(start,length)
    {
        plotter.chartData.timeSet=[]
        var nbmaxplot=plotter.chartData.datasets.length
        var nbplot=0
        for (var iPlot=0;iPlot < nbmaxplot;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id !== -1)
            {
                nbplot++
                plotData.data=[]          
                for (var i=start;i < start+length;i++)
                {
                    // compute display unit if defined
                    if(plotter.chartData.datasets.displayUnit!==undefined)
                    {
                        var value=plotData.set[i]*plotter.chartData.datasets.displayUnit.arg1
                                +plotter.chartData.datasets.displayUnit.arg2

                        plotData.data.push(value)
                    }
                    else
                    {
                        plotData.data.push(plotData.set[i])
                    }
                    if (nbplot===1)
                        plotter.chartData.timeSet.push(dataSetTime[i])
                }
            }
        }

        if (nbplot >0) // at least one plot
        {
            plotter.repaint()
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // length : size of view
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function shiftView(start,length)
    {
        var once=0
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id !==-1 )
            {
                once++
                plotData.data=[]
                if (once===1) plotter.chartData.timeSet=[]
                for (var i=start;i < start+length && i<dataSetTime.length;i++)
                {
                    if (once===1) plotter.chartData.timeSet.push(dataSetTime[i])
                    // compute display unit if defined
                    if(plotter.chartData.datasets.displayUnit!==undefined)
                    {
                        var value=plotData.set[i]*plotter.chartData.datasets.displayUnit.arg1
                                +plotter.chartData.datasets.displayUnit.arg2

                        plotData.data.push(value)
                    }
                    else
                    {
                        plotData.data.push(plotData.set[i])
                    }
                }
            }
        }
        if(once>0) plotter.repaint()
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // draw a zoom rectangle
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function drawRect(startx,starty,endx,endy)
    {
        plotter.drawRect(startx,starty,endx,endy)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // serialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function serialize(index)
    {
        var str=""
        for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
        {
            var plotData=plotter.chartData.datasets[iPlot]
            if (plotData.id!==-1)
            {
                str += "\n\t\t<plotter>"
                str += "\n\t\t\t<name>"+objectName+"</name>"
                str += "\n\t\t\t<chartindex>" + index + "</chartindex>"
                str += "\n\t\t\t<data>"+plotData.label+"</data>"
                str += "\n\t\t\t<datasetFill>"+options["datasetFill"]+"</datasetFill>"
                str += "\n\t\t\t<steppedLine>"+options["steppedLine"]+"</steppedLine>"
                str += "\n\t\t\t<pointDot>"+options["pointDot"]+"</pointDot>"
                str += "\n\t\t</plotter>\n"
            }
        }
        return str
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // update the variable list
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updatePlotDataList(check)
    {
        var modelIndexes=getModelIndexes()
        var colors=getColors()
        for (var index=0;index < modelIndexes.length;index++)
        {
            FbsfDataModel.check(modelIndexes[index],check)
            if (check)FbsfDataModel.color(modelIndexes[index],colors[index])
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Contex Menu to remove charts
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Menu { id: contextMenu;
        signal itemClicked (int modelIndex)
        function show()
        {
            clear()
            for (var iPlot=0;iPlot < plotter.chartData.datasets.length;iPlot++)
            {
                var plotData=plotter.chartData.datasets[iPlot]
                if (plotData.id!==-1)add(plotData.label,plotData.id)
            }
            contextMenu.popup()
        }
        function add(label,index)
        {
               addItem(label).triggered.connect(function(){itemClicked(index)})
        }
        onItemClicked:
        {
            removePlot(modelIndex)
            FbsfDataModel.check(modelIndex,false)
            FbsfDataModel.updateTableView()
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Chart mouse area
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Tooltip{id:tooltip}


    MouseArea
    {   id: event
        anchors.fill: parent
        acceptedButtons: isPopup ? Qt.LeftButton : Qt.LeftButton | Qt.RightButton

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // draw zoom rectangle area
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        property point pstart
        onClicked: {
            // plot unit selection
            if (selectedPlot === undefined || selectedPlot===parent ) return // GRO 2021/01/12

            selectedPlot.selected=false;            // GRO 2021/01/12
            selectedPlot.updatePlotDataList(0)      // GRO 2021/01/12

            parent.selected=true;selectedPlot=parent;
            // update selected data in list
            selectedPlot.updatePlotDataList(1)
            FbsfDataModel.updateTableView()
            currentPlotArea.repaint()
        }
        onPressed:  {
            if( pressedButtons & Qt.LeftButton)
            {   // draw zoom rectangle
                pstart.x=mouseX;pstart.y=mouseY
            }
            else if( pressedButtons & Qt.RightButton)
            {   // show remove menu
                contextMenu.show()
            }
        }
        onReleased: {
            if (mouse.button === Qt.RightButton)return
            if (Math.abs(mouseX-pstart.x)< 20 || Math.abs(mouseY-pstart.y) < 20)
            {   // simple click
                if(!flagUpdateView)
                {
                    mouseEvent.type = "releasemove"
                    mouseEvent.clientX = mouse.x;
                    mouseEvent.clientY = mouse.y;
                    if (plotter.chart!==undefined)
                    {
                        var point=plotter.chart.getPointFromMouseX(event.mouseEvent);
                        if(point!==undefined)
                        {
                            tableView.selectRow(point.index)
                            currentPlotArea.setXTimeLine(point.value)
                        }
                    }
                }
            }
            else
            {   // Draw zoom rectangle
                parent.drawRect(pstart.x,pstart.y,pstart.x,pstart.y)

                var wstart = viewStart + Math.floor(viewSize  * (pstart.x/parent.width))
                var wsize = viewSize * Math.abs(mouseX-pstart.x)/parent.width

                if (wsize>2)// at least 2 points
                {
                    viewSize=wsize
                    currentPlotArea.positionView(wstart);
                }
                else
                {
                    viewSize=2
                    repaint()
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // zoom/unzoom with mouse wheel
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        property real lastWheelTime: 0

        onWheel:
        {
            if (wheel.angleDelta.y > 0)
            {// unzoom
                viewSize += viewSize/2
                if (viewSize > fullSize) viewSize=fullSize
            }
            else
            {// zoom
                if (viewSize*0.9 > 2)
                {
                    viewSize *= 0.9
                    viewStart += viewSize*0.05
                }
                else viewSize=2
            }
            var now = new Date().valueOf()
            if (now - lastWheelTime > 100)
            {
                currentPlotArea.positionView(viewStart);
            }
            lastWheelTime = now
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // detection of dot for tooltip
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        hoverEnabled: true

        property QtObject mouseEvent: QtObject {
            property int left: 0
            property int top: 0
            property int clientX: 0
            property int clientY: 0
            property string type: ""
        }
        property point tooltipPoint: "0,0"
        onPositionChanged: {
            if (pressed)//zoom area rectangle
            {
                parent.drawRect(pstart.x,pstart.y,mouseX,mouseY)
            }
            else if(!flagUpdateView)
            {
                mouseEvent.type = "mousemove"
                mouseEvent.clientX = mouse.x;
                mouseEvent.clientY = mouse.y;
                mouseEvent.left = 0;
                mouseEvent.top = 0;
                if (plotter.chart!==undefined)
                {
                    var point=plotter.chart.bindMouseEvent(event.mouseEvent);
                }
                if(point!==undefined)
                {
                    if(tooltipPoint.x!==point.x && tooltipPoint.y!==point.y)
                    {
                        tooltipPoint.x=point.x
                        tooltipPoint.y=point.y
                        tooltip.text="name : "+ point.name
                                    +"\ntime : "+point.valx
                                    +"\nvalue : "+point.valy.toFixed(2);
                        var xOffset=((parent.width-point.x)/point.x) > 1 ? 20 : -100
                        var yOffset=((parent.height-point.y)/point.y) > 1 ? -20 : 60
                        tooltip.x=point.x+xOffset;tooltip.y=point.y-yOffset;
                        tooltip.color = "white"
                        tooltip.show()
                    }
                }
                else
                {
                    tooltipPoint.x=0
                    tooltipPoint.y=0
                    tooltip.hide()
                }
            }
        }
        onExited: {
            mouseEvent.type = "mouseout"
            mouseEvent.clientX = 0;
            mouseEvent.clientY = 0;
            mouseEvent.left = 0;
            mouseEvent.top = 0;
            tooltip.hide()
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Chart component
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QChartJs {
        id: plotter
        anchors.fill: parent
        chartType: 1
        chartOptions: options
        Component.onCompleted: {
            //Set data visual properties and data/label variables.
            chartData = {
                xmarker:0,
                xTimeLine:0,
                timeSet: [],
                labelsNumber:5,
                displayUnit : undefined,
                datasets: [
                    {
                        label   :"",
                        unit    :"",
                        id      : -1,
                        fillColor: "rgba(20,250,250,0.5)",
                        strokeColor: "Aqua" ,
                        pointColor: "Aqua",
                        pointStrokeColor: "#fff",
                        pointHighlightFill: "#fff",
                        pointHighlightStroke: "rgba(220,220,220,1)",
                        resetFutur : false,
                        set : [],     // full data set
                        data: []      // visible data set
                    },
                    {
                        label   :"",
                        unit    :"",
                        id      : -1,
                        fillColor: "rgba(250,250,250,0.3)",
                        strokeColor: "Yellow",
                        pointColor: "Yellow",
                        pointStrokeColor: "#fff",
                        pointHighlightFill: "#fff",
                        pointHighlightStroke: "rgba(220,220,220,1)",
                        resetFutur : false,
                        set : [],     // full data set
                        data: []      // visible data set
                    },
                    {
                        label   :"",
                        unit    :"",
                        id      : -1,
                        fillColor: "rgba(100,250,100,0.3)",
                        strokeColor: "DeepSkyBlue",
                        pointColor: "DeepSkyBlue",
                        pointStrokeColor: "#fff",
                        pointHighlightFill: "#fff",
                        pointHighlightStroke: "rgba(220,220,220,1)",
                        resetFutur : false,
                        set : [],     // full data set
                        data: []      // visible data set
                    },
                    {
                        label   :"",
                        unit    :"",
                        id      : -1,
                        fillColor: "rgba(250,150,200,0.3)",
                        strokeColor: "Lime",
                        pointColor: "Lime",
                        pointStrokeColor: "#fff",
                        pointHighlightFill: "#fff",
                        pointHighlightStroke: "rgba(220,220,220,1)",
                        resetFutur : false,
                        set : [],     // full data set
                        data: []      // visible data set
                    },
                    {
                        label   :"",
                        unit    :"",
                        id      : -1,
                        fillColor: "rgba(240,220,30,0.3)",
                        strokeColor: "Pink",
                        pointColor: "Pink",
                        pointStrokeColor: "#fff",
                        pointHighlightFill: "#fff",
                        pointHighlightStroke: "rgba(220,220,220,1)",
                        resetFutur : false,
                        set : [],     // full data set
                        data: []      // visible data set
                    },
                    {
                        label   :"",
                        unit    :"",
                        id      : -1,
                        fillColor: "rgba(100,40,10,0.2)",
                        strokeColor: "Orange",
                        pointColor: "Orange",
                        pointStrokeColor: "#fff",
                        pointHighlightFill: "#fff",
                        pointHighlightStroke: "rgba(220,220,220,1)",
                        resetFutur : false,
                        set : [],     // full data set
                        data: []      // visible data set
                    }
                ]
            }
        }
    }
}
