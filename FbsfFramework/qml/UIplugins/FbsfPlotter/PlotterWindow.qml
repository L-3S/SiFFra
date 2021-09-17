import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import QtQuick.XmlListModel 2.0

import fbsfplugins 1.0
import FbsfUtilities 1.0
import "qrc:/qml/Components"//Chart XY
import "qrc:/qml/Javascript/Common.js" as Util

Rectangle
{
    id : plotWindow
    anchors.fill : parent
    color: "lightgrey"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Component implementation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property int    stdViewSize     : 100       // standard mode
    property int    fullSize        : 0         // data history size
    property int    futurSize       : 0         // time depend mode (project setting)

    property var    currentPlotArea             // selected plot area
    property bool   isPopup         : false     // create or not as a popup
    property bool   isStandard      : true      // mode time depend or not
    property var    timeBase                    // time of data

    property int    shiftValue      : 0         // shift value for view
    property int    shiftStep       : 1         // step for window shift

    property bool   flagUpdateView  : false     // continuous update mode flag

    property bool   isEmpty         : true      // no plot drawing  indicator

    property double lastRefreshTime : 0         // memory for last time refresh

    property color  paletteColor    :"#666676"

    property string timeFormat      : ""
    property string timeStartUTC    : ""

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeReal       { id : timeBase1 ; tag1 : "Simulation.Time"}
    SubscribeVectorInt  { id : timeBase2 ; tag1 : "Data.Time"}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // LIFE START
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Component.onCompleted:
    {
        // switch on the relevant time base according the mode
        isStandard=FbsfDataModel.isUnresolved("Data.Time") ? true : false
        timeBase=isStandard?timeBase1:timeBase2
        currentPlotArea.viewSize=isStandard ? stdViewSize : timeBase.data.length
        if (!isStandard)
            Util.setFormatDateUTC() // UTC format from application code
        // import display unit data
        loadDisplayUnit()
        //console.log("PlotterWindow::onCompleted")
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initial time settings from configuration
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function timeSettings(format,time)
    {
        if (format==="utc")
        {
            if (time==="")
                Util.setFormatDateUTC(new Date()) // UTC format from now
            else
                Util.setFormatDateUTC(new Date(time)) // UTC fixed
            console.log("[Plotter] UTC time settings starting from",Util.getStartTime())
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Slot triggered from executive mode and status changes
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function statusChanged(mode,state)
    {
        if (state==="stepEnd" )
        {
            if ( timeBase.history.length>0)
            {
                futurSize= isStandard ? 0:timeBase.data.length-timeBase.timeindex
                fullSize = timeBase.history.length+futurSize
                if (flagUpdateView) updateWindow(1)
            }
        }
        else if (state==="paused" )
        {   // resynch plotters when paused after fast run
            currentPlotArea.updateData()
            currentPlotArea.updateView(1)
            flagUpdateView=false
        }
        else if (state==="running")
        {
            currentPlotArea.tableView.visible=false
            flagUpdateView=true
        }
        else if (state==="stepping")
        {
            flagUpdateView=true
        }
        else if (state==="reseted")// after a snapshot load
        {
            fullSize = timeBase.history.length+futurSize
            updateWindow(1)
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function updateWindow(resynch)
    {
        //if (timeBase.history.length===0) return
        if(!visible) return
        var currentTime=new Date().getTime()
        if(currentTime-lastRefreshTime < 500)return
        lastRefreshTime=currentTime
        currentPlotArea.updateData()
        currentPlotArea.updateView(resynch)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SplitView
    {
        anchors.fill: parent
        orientation: Qt.Horizontal
        handleDelegate: Rectangle { width: 2; color: "grey" }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Panel for variables display list
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Rectangle
        {   id : panelView
            width : 300
            SystemPalette {id: syspal}
            color: syspal.window
            function setFilter(role,filter)
            {
                tableView.model.filterRole=role
                tableView.model.filterCaseSensitivity= Qt.CaseInsensitive
                tableView.model.filterSyntax=FbsfFilterProxyModel.Wildcard
                tableView.model.filterString=filter
                tableView.model.setFilterWildcard(filter)
            }
            // Text for filter
            ColumnLayout{
                spacing:0
               Rectangle{ width:panelView.width
                ComboBox {id: producerFilter;
                    width:panelView.width
                    editable: false
                    model: FbsfDataModel.producers
                    onCurrentIndexChanged:
                    {
                        if (currentIndex==0)
                            {currentIndex=-1;panelView.setFilter("producer","")}
                        else
                            panelView.setFilter("producer",currentText)
                        nameFilter.text=""
                    }
                }}
               Rectangle{ width:panelView.width
                TextField {id: nameFilter
                    width:panelView.width-25
                    font.pixelSize : 16
                    placeholderText: "Name..";
                    onAccepted: {panelView.setFilter("name",text)}
                }}
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Panel for variables display
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            TableView
            {   id : tableView
                model: FbsfFilterProxyModel{id: proxyModel}
                anchors.fill : panelView
                anchors.topMargin: nameFilter.height
                headerVisible: true
                backgroundVisible: false
                alternatingRowColors: false
                sortIndicatorVisible: true
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                TableViewColumn {
                    id: checkedColumn
                    title: "";role: "checked"
                    width: 25;resizable: false
                    delegate :// a checkable rectangle
                        Rectangle {
                            anchors.fill :parent
                            border.color: "grey";border.width: 2
                            property color colorUnchecked:"lightgray"
                            property int rowIndex : proxyModel.getRow(styleData.row)
                            onRowIndexChanged: // GRO 2021/01/12
                            {
                                // update color according checked state
                                color=FbsfDataModel.checked(rowIndex)?FbsfDataModel.color(rowIndex):"lightgray"
                            }
                            property bool dataChecked:FbsfDataModel.checked(rowIndex) // GRO 2021/01/12

                            visible :( FbsfDataModel.isScalar(rowIndex)                // Scalar
                                    || FbsfDataModel.isItemVector(rowIndex)            // item data vector
                                    || FbsfDataModel.isHeadTimeDependVector(rowIndex)) // time depend
                            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                            // Variable selection
                            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                            MouseArea{anchors.fill : parent
                            onClicked: {
                                var index = proxyModel.getRow(styleData.row)
                                parent.dataChecked^=1 // toggle state
                                if (parent.dataChecked)
                                {
                                    var plotColor=currentPlotArea.registerPlot(index)

                                    if (plotColor===undefined)return

                                    parent.color=plotColor
                                    FbsfDataModel.color(index,plotColor)
                                    currentPlotArea.isEmpty=false
                                }
                                else
                                {
                                    if (currentPlotArea.chartType===1)// y=f(t)
                                    {
                                        currentPlotArea.removePlot(index)
                                        parent.color=parent.colorUnchecked
                                        FbsfDataModel.color(index,parent.colorUnchecked)
                                    }
                                }
                                FbsfDataModel.check(index,parent.dataChecked)
                                currentPlotArea.repaint()
                                updateWindow(0) //do not synch with current time
                            }
                            }
                        }
                    }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                TableViewColumn {role: "name"; title: "name"; width: 200; resizable : true}
                TableViewColumn {role: "unit"; title: "unit"; width: 100; resizable : true}
                TableViewColumn {role: "description"; title: "description"; width: 400; resizable : true}
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                onClicked:{
                    var index=proxyModel.getRow(row)
                    // expand data vector items
                    if (FbsfDataModel.isHeadDataVector(index))
                    {
                        FbsfDataModel.toggleVisible(index)
                        positionViewAtRow(row,ListView.Beginning)
                    }
                }
                headerDelegate:
                    Rectangle{
                        height : 25;
                        gradient : Gradient {
                            GradientStop {position: 0.00;color: "#958080";}
                            GradientStop {position: 0.50;color: "#d9cfcf";}
                            GradientStop {position: 1.00;color: "#958080";}
                        }
                        anchors
                        {
                            left: parent.left;right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        Text{x:10;text: styleData.value;
                            font.pixelSize : 16;font.bold: true
                        }}
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                rowDelegate:
                    Rectangle{ id : dataItem
                            property int idx : proxyModel.getRow(styleData.row)
                            height:25;border.color: "grey"
                            color: styleData.selected ? "#3499e6":"darkgrey"
                            border.width:FbsfDataModel.isHeadDataVector(dataItem.idx)?2:1
                            Image {source: FbsfDataModel.isVectorExpanded(dataItem.idx)?
                                               "icons/Collapse.png":"icons/Expand.png"
                                   x:-3;y:-3;width:30;height:width
                                   visible :  FbsfDataModel.isHeadDataVector(dataItem.idx)
                                   opacity : FbsfDataModel.isUnresolved(dataItem.idx)? 0.3:1
                            }
                    }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                itemDelegate:
                    Item{ x:10;height :25; anchors.leftMargin:10;
                    Text
                    {
                        property int idx : proxyModel.getRow(styleData.row)
                        text: styleData.value
                        font.pixelSize : 16
                        anchors.verticalCenter: parent.verticalCenter
                        color: FbsfDataModel.isParameter(idx) ?"blue":
                               FbsfDataModel.isConstant(idx)  ?"teal":
                               FbsfDataModel.isUnresolved(idx)? "red" :
                               FbsfDataModel.replayed(idx) ? "white":styleData.textColor
                    }
                }
                Component.onCompleted: {
                    FbsfDataModel.updateTableView()
                    resizeColumnsToContents()
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Plotter area with multiple plot unit
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Column {id : plotView

            ToolBar{height: 40;width: plotWindow.width-panelView.width
                RowLayout {anchors.fill:parent;spacing : 15
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { id:btSave
                        iconSource: "icons/Save.png";
                        tooltip:"Save configuration"
                        Rectangle  {
                            color:"transparent";
                            anchors.fill: parent;anchors.margins: parent.pressed?-2:-1
                            border.color:parent.pressed?"grey":"lightgrey";
                            border.width:parent.pressed?3:1}
                        onClicked: {
                            fileDialog.nameFilters=["XML files (*.xml)"]
                            fileDialog.title="Save"
                            fileDialog.selectExisting=false
                            fileDialog.open()
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: "icons/Open.png";
                        tooltip:"Open configuration"
                        Rectangle  {id : btOpen;
                            color:"transparent";
                            anchors.fill: parent;anchors.margins: parent.pressed?-2:-1
                            border.color:parent.pressed?"grey":"lightgrey";
                            border.width:parent.pressed?3:1}
                        onClicked: {
                            fileDialog.nameFilters=["XML files (*.xml)"]
                            fileDialog.title="Open"
                            fileDialog.selectExisting=true
                            fileDialog.open()
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { id:btAddFT
                        iconSource: "icons/Chart1-add.png";
                        tooltip:"Add a f(t) chart"
                        onClicked:
                        {
                            tabView.addTab("Plot"+tabView.counter,pFT)
                            tabView.currentIndex=tabView.count-1
                            tabView.counter++
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { id:btAddXY
                        iconSource: "icons/Chart2-add.png";
                        tooltip:"Add a XY chart"
                        onClicked:
                        {
                            tabView.addTab("Plot"+tabView.counter,pXY);
                            tabView.currentIndex=tabView.count-1
                            tabView.counter++
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton {id:btSavePng
                        iconSource: "icons/SavePng.png";
                        tooltip:"Save plotters to png file"
                        enabled: !flagUpdateView&& !currentPlotArea.isEmpty
                        opacity: enabled ? 1.0:0.4
                        onClicked:
                        {
                            fileDialog.nameFilters=["PNG files (*.png)","JPEG files (*.jpg)"]
                            fileDialog.title="Save png"
                            fileDialog.selectExisting=false
                            fileDialog.open()
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { id:btSaveCsv
                        iconSource: "icons/SaveCsv.png";
                        tooltip:"Save to csv file"
                        enabled: !flagUpdateView&& !currentPlotArea.isEmpty
                        opacity: enabled ? 1.0:0.4
                        onClicked:
                        {
                            fileDialog.nameFilters=["CSV files (*.csv)"]
                            fileDialog.title="Save csv"
                            fileDialog.selectExisting=false
                            fileDialog.open()
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { id:btLoadCsv
                        iconSource: "icons/LoadCsv.png"
                        tooltip:"Load csv file"
                        enabled: !flagUpdateView
                        opacity: enabled ? 1.0:0.4
                        onClicked:
                        {
                            fileDialog.nameFilters=["CSV files (*.csv)"]
                            fileDialog.title="Load csv"
                            fileDialog.selectExisting=true
                            fileDialog.open()
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton {id:btTableView
                        iconSource: "icons/TableView.png";
                        tooltip:"Show table view"
                        enabled:!currentPlotArea.isEmpty && !flagUpdateView
                        onClicked:
                        {
                            currentPlotArea.tableView.visible^=1
                            currentPlotArea.updateView(0)
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: flagUpdateView? "icons/Stop.png"
                                                           : "icons/Start.png"
                        tooltip:"Freeze display"
                        visible:currentPlotArea.chartType===1// y=f(t)
                        onClicked: {
                            flagUpdateView=flagUpdateView?false:true
                            if (flagUpdateView)
                                currentPlotArea.tableView.visible=false
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: "icons/HidePlot.png";
                        visible:currentPlotArea.chartType===1// y=f(t)
                        enabled : currentPlotArea.plotUnitNumber > 1
                        opacity: enabled ? 1.0:0.4

                        Rectangle  {
                            id : btHideSubPlot;
                            color:"transparent"
                            anchors.fill: parent
                            anchors.margins: parent.pressed?-2:-1
                            border.color:parent.pressed? "grey" : "lightgrey"
                            border.width:parent.pressed?3:1
                        }
                        onClicked: {
                            //Get index of selected subplot
                            let index = currentPlotArea.plotUnitNumber - 1;
                            for(var i=0; i<currentPlotArea.plotterUnitList.length ; i++){
                                if(currentPlotArea.plotterUnitList[i] === currentPlotArea.selectedPlot)
                                    index = i;
                            }
                            currentPlotArea.setVisible(index,false)
                        }
                    }
                    ToolButton { iconSource: "icons/ShowPlot.png";
                        visible:currentPlotArea.chartType===1// y=f(t)
                        enabled : currentPlotArea.plotUnitNumber < 4
                        opacity: enabled ? 1.0:0.4
                        Rectangle  {
                            id : btAddSubPlot;
                            color:"transparent"
                            anchors.fill: parent
                            anchors.margins: parent.pressed?-2:-1
                            border.color:parent.pressed? "grey" : "lightgrey"
                            border.width:parent.pressed?3:1
                        }
                        onClicked: {
                            //Get index of the first non visible subplot
                            let index = 0;
                            for(var i=currentPlotArea.plotterUnitList.length - 1; i>=0 ; i--){
                                if(!currentPlotArea.plotterUnitList[i].visible)
                                    index = i;
                            }
                            currentPlotArea.setVisible(index,true)
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton {iconSource: "icons/FillMode.png";
                                visible:currentPlotArea.chartType===1// y=f(t)
                                enabled: !currentPlotArea.isEmpty
                                tooltip:"Switch to fill mode"
                        Rectangle  {id : btFill;
                                    color:"transparent";
                                    anchors.fill: parent;anchors.margins: parent.pressed?-2:-1
                                    border.color:parent.pressed?"grey":"lightgrey";
                                    border.width:parent.pressed?3:1}
                                 onClicked:
                                 {
                                     currentPlotArea.selectedPlot.options["datasetFill"]=
                                     currentPlotArea.selectedPlot.options["datasetFill"]===true? false:true
                                     currentPlotArea.repaint()
                                 }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: "icons/SteppedLineMode.png";
                                 tooltip:"Switch to stepped mode"
                                 visible:currentPlotArea.chartType===1// y=f(t)
                                 enabled: !currentPlotArea.isEmpty
                        Rectangle  {id : btStepped;
                                color:"transparent";
                                anchors.fill: parent;anchors.margins: parent.pressed?-2:-1
                                border.color:parent.pressed?"grey":"lightgrey";
                                border.width:parent.pressed?3:1}
                                 onClicked:
                                 {
                                     currentPlotArea.selectedPlot.options["steppedLine"]=
                                     currentPlotArea.selectedPlot.options["steppedLine"]===true? false:true
                                     currentPlotArea.repaint()
                                 }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: "icons/PointDotMode.png";
                                 tooltip:"Switch to dot mode"
                                 visible:currentPlotArea.chartType===1// y=f(t)
                                 enabled: !currentPlotArea.isEmpty
                        Rectangle  {id : btDot;
                                color:"transparent";
                                anchors.fill: parent;anchors.margins: parent.pressed?-2:-1
                                border.color:parent.pressed?"grey":"lightgrey";
                                border.width:parent.pressed?3:1}
                                 onClicked:
                                 {
                                     currentPlotArea.selectedPlot.options["pointDot"]=
                                     currentPlotArea.selectedPlot.options["pointDot"]===true? false:true
                                     currentPlotArea.repaint()
                                 }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: "icons/FullSize.png";
                                 tooltip:"Set full range"
                                 visible:currentPlotArea.chartType===1// y=f(t)
                                 enabled: !currentPlotArea.isEmpty
                                 onClicked:
                                 {
                                     currentPlotArea.viewSize=fullSize
                                     currentPlotArea.positionView(0)
                                     updateWindow(0)
                                 }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: "icons/ShiftLeft.png";
                                 tooltip:"Shift left"
                                 visible:currentPlotArea.chartType===1// y=f(t)
                                 enabled : currentPlotArea.boundedLeft? false:true
                                 opacity: enabled ? 1.0:0.4
                                 onClicked:
                                 {
                                     shiftStep=Math.ceil(currentPlotArea.viewSize * currentPlotArea.viewSize/fullSize)
                                     currentPlotArea.shiftView(shiftValue);
                                 }
                                 onPressedChanged:
                                 {
                                     shiftStep=Math.ceil(currentPlotArea.viewSize * currentPlotArea.viewSize/fullSize)
                                     shiftValue=-shiftStep;viewShifter.running=pressed
                                 }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    Slider {id: sliderViewRange
                        visible:currentPlotArea.chartType===1// y=f(t)
                        function setValue(start,size)
                        {
                            start = fullSize - start
                            var windowScrollAreaSize = fullSize - size
                            var windowPositionRatio
                            if (windowScrollAreaSize<=0) windowPositionRatio = 0
                            else windowPositionRatio = (start-size) / windowScrollAreaSize;
                            var trackScrollAreaSize = fullSize * (1-size/fullSize) + size
                            sliderViewRange.value = trackScrollAreaSize * windowPositionRatio;
                        }
                        height: 30;width : parent.width - x - 100
                        scale : -1
                        maximumValue : fullSize - (currentPlotArea.viewSize<fullSize?currentPlotArea.viewSize/2:0)
                        minimumValue : currentPlotArea.viewSize/2
                        updateValueWhileDragging :true
                        style: SliderStyle
                        {       id:style1
                                handle: Rectangle {id:handle
                                    height: 15;
                                    width:(sliderViewRange.width)*(currentPlotArea.viewSize<fullSize?currentPlotArea.viewSize/fullSize:1)
                                    gradient: Gradient {
                                        GradientStop {position: 0.01; color: "#737272";}
                                        GradientStop {position: 0.50;color: "#fbfafa";}
                                        GradientStop {position: 1.00;color: "#737272";}
                                    }
                                    border.color: "black";radius:3
                                }
                                groove: Rectangle {
                                    implicitHeight: 5;implicitWidth: sliderViewRange.width
                                    border.color: "black";color: "#BBB"
                                }
                        }
                        onPressedChanged:
                        {
                            if (sliderViewRange.pressed)return
                            var wstart=fullSize-value-currentPlotArea.viewSize/2
                            currentPlotArea.positionView(wstart)
                        }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    ToolButton { iconSource: "icons/ShiftRight.png";
                                 tooltip:"Shift right"
                                 visible:currentPlotArea.chartType===1// y=f(t)
                                 enabled : currentPlotArea.boundedRight? false:true
                                 opacity: enabled ? 1.0:0.4
                                 onClicked:
                                 {
                                     shiftStep=Math.ceil(currentPlotArea.viewSize * currentPlotArea.viewSize/fullSize)
                                     currentPlotArea.shiftView(shiftValue);
                                 }
                                 onPressedChanged:
                                 {
                                     shiftStep=Math.ceil(currentPlotArea.viewSize * currentPlotArea.viewSize/fullSize)
                                     shiftValue=shiftStep;viewShifter.running=pressed
                                 }
                    }
                    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                    TBButton{id: help
                        source:"../../../qml/Components/icons/button_help.png";
                        onClicked: {
                            Qt.openUrlExternally(FBSF_HOME+"/Documentation/pdf/MutPlotter.pdf")
                        }
                    }
                    Item { Layout.fillWidth: true }
                }//RowLayout
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                Timer
                {   id : viewShifter
                    interval: 200; running: false; repeat: true;
                    property int accelerator : 5;
                    onTriggered: {
                        if (accelerator%5==0) shiftValue *= 2
                        accelerator++;
                        currentPlotArea.shiftView(shiftValue);
                    }
                }
            }// ToolBar
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Plotter window tabview
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            TabView {id: tabView
                Component{id:pFT ;
                    PlotterArea{viewSize:isStandard ? stdViewSize : timeBase.data.length}}
                Component{id:pXY ;ChartXY {} }//XY
                property int counter : 2
                onCurrentIndexChanged:  indexChanged()

                function indexChanged()
                {
                        currentPlotArea.updateDataList(0)       // uncheck current selected//XY
                        currentPlotArea=getTab(currentIndex).children[0]
                        currentPlotArea.updateDataList(1)       // check current selected//XY
                        FbsfDataModel.updateTableView()
                        currentPlotArea.repaint()
                        updateWindow(0) //do not synch with current time
                }
                height  : plotWindow.height-40;
                width   : plotWindow.width-panelView.width
                // initial plot page
                Tab {title: "Plot1";active:true
                    PlotterArea{id: plotArea1;
                        Binding { target: plotWindow; property: "currentPlotArea";value: plotArea1  }
                    }
                }
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                style: TabViewStyle {
                    tab: Rectangle {
                        property color color1 : "steelblue"
                        property color color2 : "lightgrey"
                        color: styleData.selected ? color1:color2
                        border.color: styleData.selected ? "white" : "grey";
                        implicitWidth:Math.max(textTitle.width + 50 , 150)
                        implicitHeight: 25
                        radius: 3
                        TextInput {id: textTitle;font.pixelSize : 16;
                            anchors.left: parent.left
                            anchors.verticalCenter:  parent.verticalCenter
                            anchors.leftMargin:10
                            selectByMouse : true
                            mouseSelectionMode : TextEdit.SelectWords
                            enabled: styleData.index===tabView.currentIndex
                            text: styleData.title
                            color: styleData.selected ? "white" : "black"
                            onTextChanged: tabView.getTab(index).title = textTitle.text
                        }
                        Rectangle { anchors.right:parent.right
                            anchors.verticalCenter:  parent.verticalCenter;
                            anchors.rightMargin: 2
                            visible: tabView.count > 1 && styleData.index<tabView.count
                            color:"transparent";
                            Text {  id : cross;text:"x";
                                anchors.verticalCenter:  parent.verticalCenter;
                                anchors.horizontalCenter:  parent.horizontalCenter;
                                font.pixelSize : 16;color:textTitle.color}
                            MouseArea{anchors.fill:parent;hoverEnabled: true
                                onClicked: if (tabView.count>1) {// at least two
                                               // Remove tab and clear current plotters
                                               tabView.getTab(styleData.index).children[0].updateDataList(0)
                                               if(styleData.index === 0) tabView.getTab(1).children[0].updateDataList(1)
                                               else tabView.getTab(0).children[0].updateDataList(1)
                                               tabView.getTab(styleData.index).children[0].clear()
                                               tabView.removeTab(styleData.index)
                                               viewUpdater.start()
                                               tabView.currentIndex=0
                                               tabView.indexChanged()
                                           }
                                onEntered: {parent.color="red";cross.color="white"}
                                onExited:  {parent.color="transparent";cross.color="black"}
                            }
                            implicitWidth: 15;implicitHeight: 15
                        }
                    }
                }
            }
        }
    }// SplitView
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // File IO C++ utility for plotters serialisation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FbsfFileIO {
        id: plotterFile
        source: "Plotter.xml"
        onError: console.log("Error : "+msg)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // File chooser for save and load
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FileDialog
    {
        id: fileDialog
        onAccepted:
        {
            if (title=="Save")
            {
                plotterFile.backup(fileUrl)
                plotterFile.open(fileUrl)
                plotterFile.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>")
                plotterFile.write("<plotterlist>")
                var workingTab
                var pageName
                for (var i = 0; i < tabView.count; i++) {
                    workingTab = tabView.getTab(i).children[0]
                    pageName = tabView.getTab(i).title
                    plotterFile.write("\t<chart>")
                    plotterFile.write("\t<index>" + i + "</index>")
                    plotterFile.write("\t<chartname>"+pageName+"</chartname>")

                    plotterFile.write("\t<charttype>"+workingTab.chartType+"</charttype>")
                    plotterFile.write(workingTab.serialize(i))
                    plotterFile.write("\t</chart>")
                }
                plotterFile.write("</plotterlist>")
                plotterFile.close()
            }
            else if (title=="Open")
            {
                deserialize(fileDialog.fileUrl.toString())
            }
            else if (title=="Save csv")
            {
                currentPlotArea.saveCsv(fileDialog.fileUrl.toString())
            }
            else if (title=="Load csv")
            {
                currentPlotArea.loadCsv(fileDialog.fileUrl.toString())
                currentPlotArea.isEmpty=false
            }
            else if (title=="Save png")
            {
                currentPlotArea.grabToImage(function(result) {
                    result.saveToFile(fileDialog.fileUrl.toString().replace("file:///",""));
                });
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // deserialize the plot configuration
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function deserialize(filename)
    {
        plotterConfig.createObject(parent,{"source" : filename})
    }
    Component{id: plotterConfig
    XmlListModel
    {
        id:config
        query: "/plotterlist/chart"
        XmlRole { name: "charttype"; query: "charttype/number()" }
        XmlRole { name: "chartname"; query: "chartname/string()" }
        XmlRole { name: "index"; query: "index/number()" }
        property string filename:source
        onStatusChanged:
        {
            switch (status)
            {
            case XmlListModel.Ready :
                var tab
                // If already filled or from different type (f(t) or f(x)) create a new tab
                for (var index = 0; index < count; index++) {
                    if(get(index).charttype===1) // f(t)
                        tab=tabView.addTab("Plot"+tabView.counter,pFT)
                    else
                        tab=tabView.addTab("Plot"+tabView.counter,pXY)
                    tab.active=true
                    tabView.currentIndex=tabView.count-1
                    tabView.counter++
                    tab.children[0].deserialize(source, get(index).index)
                    tab.children[0].isEmpty=false
                    tab.children[0].updateDataList(0)
                    tab.title=(get(index).chartname)
                    if(index === 0){
                        for(var j = tabView.count-2 ; j >= 0 ; j--){
                            tabView.getTab(j).children[0].updateDataList(0)
                            if(j === 0) tabView.getTab(1).children[0].updateDataList(1)
                            else tabView.getTab(0).children[0].updateDataList(1)
                            tabView.getTab(j).children[0].clear()
                            tabView.removeTab(j)
                            viewUpdater.start()
                            tabView.currentIndex = 0
                        }
                    }
                    viewUpdater.start()
                }
                tabView.currentIndex = 0
                tabView.indexChanged()
                break;
            case XmlListModel.Null :
            case XmlListModel.Error :console.log("[PlotterWindow]",errorString ());
                break;
            }
        }
    }}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Timer
    {   id : viewUpdater
        interval: 100; running: false; repeat: false;
        onTriggered: {
            updateWindow(1)
            FbsfDataModel.updateTableView()
            currentPlotArea.repaint()
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // File IO C++ utility for plotters display unit conversion
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FbsfFileIO {
        id: displayUnitFile
        source: APP_HOME+"DisplayUnit.csv"
        onError: console.log("Error : "+msg)
    }
    property var    displayUnitMap  : []
    property bool   displayUnitMode : false
    function loadDisplayUnit()
    {
        if(displayUnitFile.fileExists(displayUnitFile.source))
            console.info("Loading DisplayUnit.csv file")
        else
            return;

        displayUnitFile.openRead(displayUnitFile.source)
        displayUnitMode=true
        var iLine=0;
        var line;

        while((line=displayUnitFile.readline())!=="")
        {
            iLine++
            if(/^#/.test(line)) continue // skip comments

            var ValueList = line.split(';');
            //~~~~~~~~~~ Check error case ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if(ValueList.length !== 4)
                {console.log("\tError line",iLine,"missing values");continue}
            if(displayUnitMap[ValueList[0]] !== undefined)
                {console.log("\tError line",iLine,"duplicate unit",ValueList[0]);continue}
            // regsiter a new unit with data
            displayUnitMap[ValueList[0]]={  display:ValueList[1]
                                         ,  arg1:Number(ValueList[2])
                                         ,  arg2:Number(ValueList[3])}
        }
        displayUnitFile.close()
    }
}
