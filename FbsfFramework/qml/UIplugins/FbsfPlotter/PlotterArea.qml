import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.XmlListModel 2.0

import "qrc:/qml/Javascript/Common.js" as Util

SplitView {
    property int    chartType       : 1

    property int    plotUnitNumber  : 1
    property var    selectedPlot    : plotUnit1
    property var    plotterUnitList : [plotUnit1,plotUnit2,plotUnit3,plotUnit4]

    property int    viewSize                                // Current view size
    property int    viewStart       : 0                     // X origin point
    property bool   viewStartSynch  : true                  // resynchronize X origin point

    property bool   boundedLeft     : true
    property bool   boundedRight    : false
    property bool   updateMode      : false

    property bool   isEmpty         : true
    property var    tmpChartIndex   : 0

    property var    dataSetTime     : []           // Plot unit full time base
    property int    xmarker         : 0            // Past/Futur pivot for time depend mode

    property alias  tableView       : tableView    // table data visualization

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    anchors.fill    : parent
    orientation     : Qt.Horizontal
    handleDelegate  : Rectangle { width: 2;}// color: plotWindow.color }

    Component.onCompleted:
    {
        // initialisation of the data model one row 4x6 columns
        dataModel.append({time:"",timeValue:0});
        var maxColumns=plotterUnitList.length*plotUnit1.getNbMaxPlot()
        for(var col=0; col < maxColumns;col++)
                dataModel.setProperty(0,"data"+col,"");
        plotUnit1.selected=true
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Panels for plotting area
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rectangle
    {
        border.width  : 2
        Layout.fillWidth    : true
        Column
        {   id : window
            anchors.fill: parent
            spacing:0
            PlotterUnit{id : plotUnit1
                objectName: "plotUnit1"
                width : window.width
                height :window.height/plotUnitNumber
                visible:true
            }
            PlotterUnit{id : plotUnit2
                objectName: "plotUnit2"
                width : window.width
                height :window.height/plotUnitNumber
                visible:plotUnitNumber>=2
            }
            PlotterUnit{id : plotUnit3
                objectName: "plotUnit3"
                width : window.width
                height :window.height/plotUnitNumber
                visible:plotUnitNumber>=3
            }
            PlotterUnit{id : plotUnit4
                objectName: "plotUnit4"
                width : window.width
                height :window.height/plotUnitNumber
                visible:plotUnitNumber>=4
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Panel for data Table view
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rectangle {
        id              : tableView
        implicitWidth   : 460
        height          : parent.height
        visible         : false
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        property int colSize    : 140
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Function to fill the table view
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        function fillTable(start,size)
        {
            if(visible===false) return
            dataModel.clear()
            for(var i=start;i<start+size&&i<dataSetTime.length;i++)
            {   // fill rows
                dataModel.append({time:FbsfTimeManager.dateTimeStr(dataSetTime[i]),
                                     timeValue:dataSetTime[i]})
                if (selectedValue===dataSetTime[i])
                {
                    selectRow(i-start) // when change to visible;
                }

                // fill columns
                for(var col=1;col < idTable.columnCount;col++)
                {   // iterate over table columns
                    var plotUnit=idTable.getColumn(col).plotUnit
                    var plotIndex=idTable.getColumn(col).plotIndex
                    var plotData=plotterUnitList[plotUnit].getPlotData(plotIndex)
                    if(plotData!==undefined)
                    dataModel.setProperty(i-start,idTable.getColumn(col).role,
                                          plotData.data[i-start].toFixed(3));
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Function to select a row
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        property var selectedValue : 0
        function selectRow(index)
        {
            if(visible && index>0 && index < dataModel.rowCount())
            {
                idTable.selection.clear()
                idTable.currentRow=index
                idTable.selection.select(index)
                idTable.positionViewAtRow(index, ListView.Center)
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // add a new column
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        function addColumn(modelIndex,plotUnit,plotIndex)
        {
            //Check if index is already in tableview
            for(var col=1;col < idTable.columnCount;col++)
            {
                if (idTable.getColumn(col).index===modelIndex)return
            }
            var rolename="data"+(plotUnit*plotUnit1.getNbMaxPlot()+plotIndex)
            var title=FbsfDataModel.name(modelIndex)
            idTable.addColumn(
                    columnComponent.createObject(idTable,
                                                 {
                                                   "width"      : title.length*9,
                                                   "role"       : rolename,
                                                   "title"      : title,
                                                   "index"      : modelIndex,
                                                   "plotUnit"   : plotUnit,
                                                   "plotIndex"  : plotIndex
                                                 }))
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // remove a column
        // TODO fix the following warning due to remove column
        // [Warning] : QtQuick/Controls/Styles/Desktop/TableViewStyle.qml:100:
        // TypeError: Cannot read property of null
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        function removeColumn(modelIndex)
        {
            // delete only if no more index found
            for (var p=0;p<plotterUnitList.length;p++)
                if(plotterUnitList[p].getIndex(modelIndex)>=0) return
            for(var col=1;col < idTable.columnCount;col++)
                if (idTable.getColumn(col).index===modelIndex)
                {
                    idTable.removeColumn(col);
                }
            tableView.fillTable(viewStart,viewSize)
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //Data model for Table
        ListModel {id: dataModel}
        // Component for dynamic columns
        Component{id: columnComponent;
            TableViewColumn{
                width: tableView.colSize;horizontalAlignment : Text.AlignHCenter
                property var index;property var plotUnit;property var plotIndex}}
        // Table for display data
        TableView{
            id      : idTable
            width   : tableView.width
            height  : tableView.height
            model   : dataModel

            TableViewColumn{
                property var timeValue:0
                role    : "time"  ; title: "time"
                width   : tableView.colSize ;
                visible : idTable.columnCount>1
                horizontalAlignment : Text.AlignHCenter
            }
            headerDelegate:
                Rectangle{id : headerRect;height:25;color:"teal"
                    Text{id: header
                        text: styleData.value;
                        color: {if(styleData.column <=0) return "white"
                            var plotUnit=idTable.getColumn(styleData.column).plotUnit
                            var plotIndex=idTable.getColumn(styleData.column).plotIndex
                            return plotterUnitList[plotUnit].getColor(plotIndex)
                        }
                        font.bold: true
                        font.pixelSize : 14
                        anchors.horizontalCenter: headerRect.horizontalCenter
                    }
            }
            rowDelegate:
                Rectangle{
                    height : 25
                    border.color: "grey"
                    color: styleData.selected ? "#3499e6":"lightgrey"
                }
            // Change row selection
            onClicked:{setXTimeLine(dataModel.get(row).timeValue)}
            Connections {id:cnx
                target: idTable.selection
                function onSelectionChanged()
                {
                    if (idTable.currentRow>=0 && dataModel.get(idTable.currentRow)!==undefined)
                        setXTimeLine(dataModel.get(idTable.currentRow).timeValue)
                }
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to register a plotter unit
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function registerPlot(modelIndex)
    {
        var plotIndex = selectedPlot.registerPlot(modelIndex)
        if(plotIndex !== undefined)
        {
            // Get the plot global index among all plot units
            for (var iplot=0;iplot<plotterUnitList.length;iplot++ )
            {
                if (plotterUnitList[iplot]===selectedPlot)
                    tableView.addColumn(modelIndex,iplot,plotIndex)
            }
            return selectedPlot.getColor(plotIndex)
        }
        return undefined
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to remove an existing Plot
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function removePlot(modelIndex)
    {
        selectedPlot.unsetDataSet(modelIndex)
        tableView.removeColumn(modelIndex)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to set plotter visible
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setVisible(index,state)
    {
        if (updateMode)return // mode update selector only
        if (plotUnitNumber===1 && state === false)return ;
        plotterUnitList[index].visible=state;
        if(state===true)
        {
            selectPlot(index)
            if(plotUnitNumber<plotterUnitList.length) plotUnitNumber++;
        }
        else
        {
            if(plotUnitNumber>1)plotUnitNumber--;
            for (var i=0;i<plotterUnitList.length;i++ )
                if(plotterUnitList[i].visible) selectPlot(i)
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function update datalist when tabview changed
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateDataList(check)
    {
        for(var i = 0; i < plotterUnitList.length; ++i)
        {
            if (check===0)
                plotterUnitList[i].updatePlotDataList(check)
            else if(plotterUnitList[i].selected)
                plotterUnitList[i].updatePlotDataList(check)
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to clear all plotterUnit
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function clear()
    {
        var indexList=[]
        for(var i = 0; i < plotterUnitList.length; ++i)
        {
            plotterUnitList[i].clear(indexList)
        }
        for (var index=0;index < indexList.length;index++)
        {
            FbsfDataModel.check(indexList[index],false)
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to update plot data
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateData()
    {
        if (isStandard)
        {   //~~~~~~~~~~~~~standard mode ~~~~~~~~~~~~~~~~~~~~~~~~
            dataSetTime=timeBase.history
        }
        else //~~~~~~~~~~~~~ time depend mode ~~~~~~~~~~~~~~~~~~~
        {
            // add the history and futur time base
            dataSetTime=timeBase.historywithfutur
            xmarker=dataSetTime[dataSetTime.length-futurSize-1]
        }
        for(var i = 0; i < plotterUnitList.length; ++i)
        {
            plotterUnitList[i].updateData();
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to update view window
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateView(resynch)
    {
        var nbData= fullSize < viewSize ? fullSize:viewSize
        if (resynch || viewStartSynch)
        {
            viewStart=fullSize - nbData // shift on runtime
            if (viewStartSynch==false && !isPopup) // restart after moved
                sliderViewRange.setValue(viewStart,viewSize)
            viewStartSynch=resynch  // due to slider move
        }
        if(viewStart>0)boundedLeft=false

        for(var i = 0; i < plotterUnitList.length; ++i)
        {
            plotterUnitList[i].updateView(viewStart,nbData);
        }
        tableView.fillTable(viewStart,viewSize)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function repaint()
    {
        for(var i = 0; i < plotterUnitList.length; ++i)
        {
            plotterUnitList[i].repaint();
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to shift plotter window
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function shiftView(value)
    {
        var pos=viewStart+value
        positionView(pos)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to set the position of the plotter window
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function positionView(pos)
    {
        viewStartSynch=false // do not resynchronize X origin point
        viewStart=pos
        // check bounds
        boundedLeft=false
        boundedRight=false
        var nbData= fullSize<viewSize? fullSize:viewSize

        if (viewStart <0 )
        {viewStart=0;boundedLeft=true;}
        else if (viewStart > fullSize - nbData  )
        {viewStart=fullSize - nbData ;boundedRight=true;}
        for(var i = 0; i < plotterUnitList.length; ++i)
        {
            plotterUnitList[i].shiftView(viewStart,nbData);
        }
        if (!isPopup) sliderViewRange.setValue(viewStart,viewSize)
        // update data table view
        tableView.fillTable(viewStart,viewSize)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to register selected plotter unit
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function selectPlot(index)
    {
        if (selectedPlot !== undefined)
        {selectedPlot.selected=false;selectedPlot.updatePlotDataList(0)}
        selectedPlot=plotterUnitList[index]
        plotterUnitList[index].selected=true
        // update selected data in list
        selectedPlot.updatePlotDataList(1)
        FbsfDataModel.updateTableView()
        currentPlotArea.repaint()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to update the plot selector when tabview changed
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setXTimeLine(value)
    {
        for (var i=0;i<plotterUnitList.length;i++ )
        {
            plotterUnitList[i].setXTimeLine(value)
        }
        tableView.selectedValue=value
        repaint()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // serialize a plot configuration
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function serialize(index)
    {
        var str=""
        for(var i = 0; i < plotterUnitList.length; ++i)
        {
            str+=plotterUnitList[i].serialize(index);
        }
        return str
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // deserialize a plot configuration
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function deserialize(filename, idx)
    {
        tmpChartIndex = idx
        plotterConfig.createObject(parent,{"source" : filename})
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // save to csv fie
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function saveCsv(filename)
    {
        selectedPlot.saveCsv(filename)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // load from csv fie
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function loadCsv(filename)
    {
        selectedPlot.loadCsv(filename)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component{id: plotterConfig
        XmlListModel
        {
            query: "/plotterlist/chart/plotter"
            XmlRole { name: "name"; query: "name/string()" }
            XmlRole { name: "data"; query: "data/string()" }
            XmlRole { name: "chartindex"; query: "chartindex/number()" }
            XmlRole { name: "datasetFill"; query: "datasetFill/string()" }
            XmlRole { name: "steppedLine"; query: "steppedLine/string()" }
            XmlRole { name: "pointDot"; query: "pointDot/string()" }

            onStatusChanged:
            {
                switch (status)
                {
                case XmlListModel.Ready :
                    var iPlot=-1
                    var currentPlot=""
                    plotUnitNumber=0
                    var p=-1
                    for(var i=0;i< count ; i++)
                    {
                        if (get(i).chartindex !== tmpChartIndex)
                            continue;
                        var index=FbsfDataModel.modelIndex(get(i).data)
                        if (index === -1) continue // data not found
                        if(get(i).name!==currentPlot)
                        {
                            currentPlot=get(i).name
                            plotUnitNumber++
                            p++
                            plotterUnitList[p].options["datasetFill"] = get(i).datasetFill === "true"
                            plotterUnitList[p].options["steppedLine"] = get(i).steppedLine === "true"
                            plotterUnitList[p].options["pointDot"] = get(i).pointDot === "true"
                        }
                        selectedPlot=plotterUnitList[p]
                        var color=registerPlot(index)
                        plotterUnitList[p].selected=false
                        FbsfDataModel.check(index,1)
                        FbsfDataModel.color(index,color)
                    }
                    if (plotUnitNumber==0) plotUnitNumber ++;

                    break;
                case XmlListModel.Null :
                case XmlListModel.Error :console.log("[PlotterArea]",errorString ());
                    break;
                }
            }
        }
    }
}


