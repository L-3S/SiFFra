import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.1
import QtQuick.XmlListModel 2.0
import "qrc:/qml/Javascript/Common.js" as Util
import FbsfUtilities 1.0

Rectangle {
    id : plotArea
    anchors.fill:parent
    property int    chartType       : 2
    objectName                      : "plotXY"

    property int    tmpChartIndex   : 0
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Graphic properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property color  bgColor         : "teal"
    property color  yScaleColor     : "lightblue"
    property color  xScaleColor     : "lime"

    property int    xRange          : 100   // Plot window range value
    property int    xAxisOrigin     : 0     // used to shift values on x axis

    property color  gridColor       : "darkgrey"
    property color  axisColor       : "black"

    property int    gridLineWidth   : 1
    property int    scaleLineWidth  : 1
    property int    axisLineWidth   : 1

    property int    xPadding        : 60    // for vertical scale
    property int    yPadding        : 80    // for horizontal scale
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property real   xMinValue       : Number.MAX_VALUE
    property real   xMaxValue       : -Number.MAX_VALUE

    property int    vTickWidth      : 10;
    property int    numVTicks       : 10;
    property int    hTickWidth      : 10;
    property int    numHTicks       : 10;

    property int    serieCount      : 0
    property var    xDataSerie      : []
    property var    yDataSerie      : []
    property var    serieColor      : ["lime","yellow","orange"]
    property var    indexList       : []

    property bool   isEmpty         : true

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Container for all the created Plot instances.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Item { id: plotContainer }

    property alias  tableView       : tableView    // table data visualization

    SplitView{
        anchors.fill:parent
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Canvas drawing
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Canvas
        {
            id:canvas
            Layout.fillWidth    : true
            antialiasing        : true
            renderStrategy      : Canvas.Threaded

            property real   cHeight     : height;
            property real   cWidth      : width;
            property var    gradient
            property int    vTickSpacing : (cHeight - yPadding*2)/(numVTicks);
            property int    hTickSpacing : (cWidth  - xPadding*2)/(numHTicks);
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Component.onCompleted: {}

            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // X and Y labels
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            Item{id: xLabel;z:10;property string label:"";visible:label!=""
                anchors.horizontalCenter:parent.horizontalCenter
                anchors.horizontalCenterOffset: - xPadding
                anchors.bottom : parent.bottom
                anchors.bottomMargin:yPadding/2
                Rectangle {width:10;height:width;color:serieColor[0]
                    Text{text : xLabel.label
                        anchors.left :parent.right;anchors.leftMargin : 10
                        anchors.verticalCenter : parent.verticalCenter
                        font.pixelSize: 16;font.bold:true;color : serieColor[0]}}}

            Item{id: yLeftLabel;y:yPadding/2;z:10;
                property string label:"";visible:label!=""
                anchors.left:parent.left;anchors.leftMargin: xPadding
                Rectangle {width:10;height:width;color:serieColor[1]
                    Text{text : yLeftLabel.label
                        anchors.left :parent.right;anchors.leftMargin : 10
                        anchors.verticalCenter : parent.verticalCenter
                        font.pixelSize: 16;font.bold:true;color : serieColor[1]}}}

            Rectangle{id: yRightLabel;y:yPadding/2;z:10;width:200;
                property string label:"";visible:label!=""
                anchors.right:parent.right;anchors.rightMargin: xPadding
                Rectangle {width:10;height:width;color:serieColor[2]
                    Text{id : rtxt;text : yRightLabel.label
                        anchors.left :parent.right;anchors.leftMargin : 10
                        anchors.verticalCenter : parent.verticalCenter
                        font.pixelSize: 16;font.bold:true;color : serieColor[2]}}}
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            onPaint:
            {
                var ctx = getContext('2d');
                ctx.globalCompositeOperation = "source-over";
                ctx.font = "16px sans-serif";
                gradient = ctx.createRadialGradient(0, 100, 100, 200, 0, 600);
                gradient.addColorStop(0, Qt.lighter(bgColor, 1.2));
                gradient.addColorStop(1, Qt.lighter(bgColor, 0.8));

                ctx.fillStyle = gradient;
                ctx.fillRect(0, 0, width, height);

                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Draw horizontal gridlines
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                ctx.beginPath();
                ctx.strokeStyle = gridColor;
                ctx.lineWidth = gridLineWidth;
                for(var i1 = 0; i1 < numVTicks; i1++)
                {
                    ctx.moveTo(xPadding , yPadding + vTickSpacing*i1);
                    ctx.lineTo(cWidth - xPadding, yPadding + vTickSpacing*i1);
                }
                ctx.stroke();
                ctx.closePath();
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Draw vertical gridlines
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                ctx.beginPath();
                ctx.strokeStyle = gridColor;
                ctx.lineWidth = gridLineWidth;
                for(var i2 = 0; i2 <= numHTicks; i2++)
                {
                    ctx.moveTo(xPadding + hTickSpacing*i2, yPadding );
                    ctx.lineTo(xPadding + hTickSpacing*i2, cHeight - yPadding );
                }
                ctx.stroke();
                ctx.closePath();
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Draw X Axis
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                ctx.beginPath();
                ctx.strokeStyle = xScaleColor;
                ctx.lineWidth = axisLineWidth;
                ctx.moveTo(xPadding,cHeight - yPadding);
                ctx.lineTo(cWidth - xPadding, cHeight - yPadding); // X axis
                ctx.stroke();
                ctx.closePath();
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Draw horizontal ticks on X axis
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                if(xMinValue != Number.MAX_VALUE && xMaxValue != -Number.MAX_VALUE)
                {
                    ctx.beginPath();
                    ctx.fillStyle = xScaleColor;
                    ctx.lineWidth = scaleLineWidth;
                    for(var i3 = 0; i3 <= numHTicks; i3++)
                    {
                        ctx.moveTo(xPadding + hTickSpacing*i3, cHeight - yPadding - hTickWidth/2 );
                        ctx.lineTo(xPadding + hTickSpacing*i3, cHeight - yPadding + hTickWidth/2);
                        var xTick=(((xMaxValue-xMinValue)/numHTicks)*i3 + xAxisOrigin)
                        // Compute X scale values precision
                        ctx.fillText(xTick.toFixed(getDecimalPlaces(xTick)),
                                     xPadding + hTickSpacing*i3,cHeight - yPadding + 25)
                    }
                    ctx.stroke();
                    ctx.closePath();
                }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Iterate through the children elements of the
                // container item and draw all of the plots.
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                for(var i4 = 0; i4 < plotContainer.children.length; ++i4)
                {
                    plotContainer.children[i4].drawPlot(ctx)
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Panel for data Table view
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Rectangle
        {
            id              : tableView
            implicitWidth   : idTable.columnCount*200
            height          : parent.height
            visible         : false
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            property int colSize    : 200
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Function to fill the table view
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function fillTable()
            {
                if(visible===false) return
                dataModel.clear()
                for(var i=0;i<xDataSerie.length;i++)
                {   // fill rows
                    dataModel.append({xData:xDataSerie[i].toFixed(3)})

                    // fill columns
                    for(var col=1;col < idTable.columnCount;col++)
                    {   // iterate over table columns
                        var plotData=FbsfDataModel.history(indexList[col])
                        if(plotData!==undefined)
                            dataModel.setProperty(i,idTable.getColumn(col).role,
                                                  plotData[i].toFixed(3));
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
            function addColumn(modelIndex,plotIndex)
            {
                var rolename="data"+(plotIndex)
                var title=FbsfDataModel.name(modelIndex)
                idTable.addColumn(
                            columnComponent.createObject(idTable,
                                                         {
                                                             "width"      : title.length*9,
                                                             "role"       : rolename,
                                                             "title"      : title,
                                                             "index"      : modelIndex,
                                                             "plotIndex"  : plotIndex
                                                         }))
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // remove a column
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function removeColumn(modelIndex)
            {
                for(var col=1;col < idTable.columnCount;col++)
                    if (idTable.getColumn(col).index===modelIndex)
                        idTable.removeColumn(col);

                tableView.fillTable()
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            //Data model for Table
            ListModel {id: dataModel}
            // Component for dynamic columns
            Component{id: columnComponent;
                TableViewColumn{
                    width: tableView.colSize;horizontalAlignment : Text.AlignHCenter
                    property var index;property var plotIndex}}
            // Table for display data
            TableView{
                id      : idTable
                width   : tableView.width
                height  : tableView.height
                model   : dataModel

                TableViewColumn{
                    role    : "xData"  ; title: xLabel.label
                    width   : tableView.colSize ;
                    visible : idTable.columnCount>1
                    horizontalAlignment : Text.AlignHCenter
                }
                headerDelegate:
                    Rectangle{height:25;color:"teal"
                    Text{id: header
                        text: styleData.value;
                        color: {if(styleData.column <=0) return "white"
                            var plotIndex=idTable.getColumn(styleData.column).plotIndex
                            return serieColor[plotIndex]
                        }
                        font.bold: true
                        font.pixelSize : 14
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                rowDelegate:
                    Rectangle{
                    height : 25
                    border.color: "grey"
                    color: styleData.selected ? "#3499e6":"lightgrey"
                }
            }
        }
    }//end SplitView
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function which creates a new Chart
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function addPlot(name,index,color)
    {
        newPlotObject({
                          "plotName"    :Qt.binding(function(){ return name }),
                          "plotColor"   :Qt.binding(function(){ return color}),
                          "plotIndex"   :Qt.binding(function(){ return index}),
                          "plotArea"    :plotArea
                      })
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Create Plot instances.
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function newPlotObject(propertyMap)
    {
        // "plotContainer": the parent of the new object.
        // "propertyMap": a map of property-value pairs to be set on the object.
        var plot = plotComponent.createObject(plotContainer, propertyMap)
        if (plot === null) {console.log("ERROR: creating Plot object")}
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // function to set the data serie (1:X, 2:Yleft, 3:Yright)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function registerPlot(modelIndex)
    {
        if (serieCount>=3) return undefined// max 2

        var name=FbsfDataModel.name(modelIndex)
        indexList[serieCount]=modelIndex
        var color=serieColor[serieCount]
        if (serieCount===0)
        {
            xDataSerie=FbsfDataModel.history(modelIndex);
            xLabel.label=name
        }
        else if (serieCount===1)
        {
            yDataSerie=FbsfDataModel.history(modelIndex);
            yLeftLabel.label=name
            addPlot(name,0,color)
            tableView.addColumn(modelIndex,serieCount)
            updateData()
        }
        else if (serieCount===2)
        {
            yDataSerie=FbsfDataModel.history(modelIndex);
            yRightLabel.label=name
            addPlot(name,1,color)
            tableView.addColumn(modelIndex,serieCount)
            updateData()
        }

        serieCount++

        return color
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function repaint(){canvas.requestPaint()}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateData()
    {
        if(plotContainer.children.length===0) return

        xDataSerie=FbsfDataModel.history(indexList[0])
        for(var i = 0; i < plotContainer.children.length; ++i)
        {
            yDataSerie=FbsfDataModel.history(indexList[i+1])
            var dataset=[]
            for (var x=0;x<xDataSerie.length;x++)
            {
                dataset.push(Qt.point(xDataSerie[x],yDataSerie[x]))
            }
            plotContainer.children[i].addSerie(dataset)
        }
        canvas.requestPaint()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateView()
    {
        canvas.requestPaint();
        tableView.fillTable()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function clear() {
        serieCount=0
        isEmpty=true
        xLabel.label=""
        yLeftLabel.label=""
        yRightLabel.label=""
        for(var i = 0; i < plotContainer.children.length; ++i)
        {
            plotContainer.children[i].listPoint=[]
        }
        updateDataList(false)// uncheck

        canvas.requestPaint()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // update the variable table view
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateDataList(check)
    {
        for (var index=0;index < indexList.length;index++)
        {
            FbsfDataModel.check(indexList[index],check)
            if (check)
                FbsfDataModel.color(indexList[index],serieColor[index])
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // serialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function serialize(index)
    {
        if (serieCount<1) return

        var str=""
        str += "\n\t\t<plotter>"
        str += "\n\t\t\t<name>"+objectName+"</name>"
        str += "\n\t\t\t<chartindex>" + index + "</chartindex>"
        if (serieCount>1)
            str += "\n\t\t\t<dataX>"+xLabel.label+"</dataX>"
        if (serieCount>1)
            str += "\n\t\t\t<dataY1>"+plotContainer.children[0].plotName+"</dataY1>"
        if (serieCount>2)
            str += "\n\t\t\t<dataY2>"+plotContainer.children[1].plotName+"</dataY2>"
        str += "\n\t\t</plotter>\n"

        return str
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // deserialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function deserialize(filename, index)
    {
        tmpChartIndex = index
        plotterConfig.createObject(parent,{"source" : filename})
    }
    Component{id: plotterConfig
        XmlListModel
        {    id:config
            query: "/plotterlist/chart/plotter"
            XmlRole { name: "name"; query: "name/string()" }
            XmlRole { name: "dataX"; query: "dataX/string()" }
            XmlRole { name: "chartindex"; query: "chartindex/number()" }
            XmlRole { name: "dataY1"; query: "dataY1/string()" }
            XmlRole { name: "dataY2"; query: "dataY2/string()" }

            onStatusChanged:
            {
                switch (status)
                {
                case XmlListModel.Ready :
                    for(var i=0;i< count ; i++) {
                        if (get(i).chartindex !== tmpChartIndex)
                            continue;
                        var indexX=FbsfDataModel.modelIndex(get(i).dataX)
                        if (indexX===-1) break // data not found
                        var indexY1=FbsfDataModel.modelIndex(get(i).dataY1);
                        if (indexX===-1) break // data not found

                        registerPlot(indexX)
                        registerPlot(indexY1)

                        if (get(i).dataY2!=="")
                        {
                            var indexY2=FbsfDataModel.modelIndex(get(i).dataY2)
                            if (indexX===-1) break // data not found
                            registerPlot(indexY2)
                        }
                    }
                    isEmpty=false
                    canvas.requestPaint()
                    break;
                case XmlListModel.Null :
                case XmlListModel.Error :console.log("[PlotterLoader]",errorString ());
                    break;
                }
            }
        }}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Plot line item
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Component{id:plotComponent
        Item{
            property string     plotName        : "unnamed"
            property int        plotIndex       : 0
            property color      plotColor       : "lightblue"
            property variant    plotArea        : undefined
            property int        plotLineWidth   : 2

            property real       xMax            : -Number.MAX_VALUE
            property real       xMin            : Number.MAX_VALUE;
            property real       yMax            : -Number.MAX_VALUE
            property real       yMin            : Number.MAX_VALUE

            property real       xMaxNorm
            property real       xMinNorm
            property real       yMaxNorm
            property real       yMinNorm

            property var        listPoint       : []

            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function addSerie(dataset)
            {
                listPoint=dataset

                for(var i = 0;i < listPoint.length ;i++)
                {
                    // autofit X scale and normalize min and max
                    if (listPoint[i].x > xMax ){ xMax = listPoint[i].x;}
                    if (listPoint[i].x < xMin ){ xMin = listPoint[i].x;}
                    // autofit Y scale and normalize min and max
                    if (listPoint[i].y > yMax ){ yMax = listPoint[i].y;}
                    if (listPoint[i].y < yMin ){ yMin = listPoint[i].y;}
                }
                var valueRange = xMax - xMin;
                xMaxNorm=normalize(xMax,valueRange,true)
                xMinNorm=normalize(xMin,valueRange,false)
                valueRange = yMax - yMin;
                yMaxNorm=normalize(yMax,valueRange,true)
                yMinNorm=normalize(yMin,valueRange,false)

                xAxisOrigin = xMinNorm
                xRange = xMaxNorm - xMinNorm
                // autofit on X scale
                plotArea.xMinValue=xMinNorm
                plotArea.xMaxValue=xMaxNorm
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function drawPlot(ctx)
            {
                var plotWidth   = ctx.canvas.width  - plotArea.xPadding * 2
                var plotHeight  = ctx.canvas.height - plotArea.yPadding * 2;
                var xPos        = 0
                var yPos        = 0

                ctx.strokeStyle = plotColor
                ctx.lineWidth   = plotLineWidth;
                ctx.beginPath();

                for(var i = 0;i< listPoint.length;i++)
                {
                    // x coordonate is computed as canvas width*(x - xmin / xRange)
                    xPos = plotWidth*((listPoint[i].x - xMinNorm)/xRange)+plotArea.xPadding;
                    // y coordonate is computed as canvas height*(1 - (delta y / range))
                    yPos = plotHeight * ( 1 - ((listPoint[i].y - yMinNorm)/(yMaxNorm - yMinNorm)))
                    yPos += plotArea.yPadding
                    ctx.beginPath();
                    ctx.arc(xPos, yPos, 2, 0, 2 * Math.PI, true);
                    ctx.stroke();
                }
                ctx.stroke();
                ctx.closePath();

                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Draw vertical Y axis
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                var xPosAxis
                var labelOffset
                if (plotIndex==0){xPosAxis=xPadding;labelOffset=5}
                else{xPosAxis=canvas.width - xPadding ;labelOffset=xPosAxis+5;}
                ctx.beginPath();
                ctx.strokeStyle = plotColor;
                ctx.lineWidth = axisLineWidth;
                ctx.moveTo(xPosAxis,yPadding);
                ctx.lineTo(xPosAxis,canvas.height - yPadding);
                ctx.stroke();
                ctx.closePath();
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Draw vertical ticks on Y axis
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                ctx.beginPath();
                ctx.fillStyle = plotColor;
                ctx.lineWidth = scaleLineWidth;

                for(var i1 = 0; i1 <= numVTicks; i1++)
                {
                    ctx.moveTo(xPosAxis - vTickWidth/2, yPadding + canvas.vTickSpacing*i1);
                    ctx.lineTo(xPosAxis + vTickWidth/2, yPadding + canvas.vTickSpacing*i1);
                    var yTick=(((yMaxNorm-yMinNorm)/numVTicks)*(numVTicks-i1) + yMinNorm)
                    // Compute Y scale values precision
                    ctx.fillText(yTick.toFixed(getDecimalPlaces(yTick)),
                                 labelOffset,yPadding + canvas.vTickSpacing *i1)
                }
                ctx.stroke();
                ctx.closePath();

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Compute normalized Y values for min and max
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function normalize(value,range,isMax)
            {
                if (value === 0)
                {
                    if (isMax) return value
                    else return value - 1
                }
                var rangeOrderOfMagnitude = Math.floor(Math.log(range) / Math.LN10)
                var divider=Math.pow(10,rangeOrderOfMagnitude)
                var valueNorm;
                if (isMax)
                    valueNorm= Math.ceil(value/divider)*divider
                else
                    valueNorm= Math.floor(value/divider)*divider
                return valueNorm
            }
        }//endItem
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Save active plots values as csv file
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // File IO C++ utility for plotters serialization
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FbsfFileIO {
        id: csvFile
        source: "Plotter.csv"
        //onError: Util.messageBox("ERROR",msg)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // save to csv fie
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function saveCsv(filename)
    {
        if(csvFile.open(filename)===false) return
        // write plot names
        var names=xLabel.label
        names+= ";" + yLeftLabel.label
        if (serieCount==3) names+= ";" + yRightLabel.label
        csvFile.write(names)

        // write plot data
        var points=plotContainer.children[0].listPoint

        for (var i=0;i < points.length;i++)
        {
            var data=""
            data+= points[i].x + ";" + points[i].y
            if (serieCount==3)data+= ";" + plotContainer.children[1].listPoint[i].y
            csvFile.write(data)
        }
        csvFile.close()
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // load from csv fie
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function loadCsv(filename)
    {
        Util.messageBox("ERROR","Loading csv currently not implemented")
        // currently not implemented
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Compute normalized Y values for min and max
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getDecimalPlaces (num) {
        num=Math.round(num*1000)/1000 // limit precision
        if (num%1!=0 && num.toString().split(".")[1]!==undefined) {
            return num.toString().split(".")[1].length
        } else {
            return 0;
        }
    }
}
