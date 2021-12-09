//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Object : This object manage plugin dinamically inserted
//
//  Public properties :
//
//          appIcons : list model to append toolbar icons
//
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
import QtQuick 2.0
import QtQuick.Dialogs 1.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.12
import QtQuick.Controls.Styles 1.4
import QtQuick.XmlListModel 2.0
import fbsfplugins 1.0

import "qrc:/qml/Javascript/Common.js" as Util

ToolBar
{
    objectName: "AppToolbar"
    property variant framework
    property variant pluginframework
    property int currentPluginIndex:0
    property bool first: true
    property bool busy: false

    property var timeStep
    property var timeShift
    property var pastSize: 0
    property var futurSize: 0

    property bool hidePauseBtn:false
    property bool hideStepBtn:false
    property bool hideRunBtn:false
    property bool hideSpeedBtn:false
    property bool hideStepCrtBtn:false
    property bool hideSnapControlBtn:false
    property bool hideNavigationBtn:false
    property bool hideBacktrackBtn:false

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Slot notification of executive mode and status changes
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function statusChanged(mode,state)
    {
        if(state==="error")
            warningButton.visible=true
        if (mode==="fullreplay" && state==="initialized")
        {
            backtrackButton.enabled=false
            recordButton.visible=false
            replaySlider.visible=true
            saveButton.enabled=false
            restoreButton.enabled=false
            replaySlider.maximumValue=FbsfTimeManager.historySize()-1
            nbSteps.enabled=false
            stepToExit.enabled=false
            stepToPause.enabled=false
        }
        if (mode==="partialreplay" && state==="initialized")
        {
            backtrackButton.enabled=false
            saveButton.enabled=false
            restoreButton.enabled=false
            nbSteps.enabled=false
            stepToExit.enabled=false
            stepToPause.enabled=false
        }

        if(state==="paused")
        {
            pauseButton.enabled=false
            backtrackButton.enabled=FbsfTimeManager.historySize()-1 >= 1 ? true : false
            runButton.enabled=true
            speedButton.enabled=true
            stepButton.enabled=true
            saveButton.enabled=true
            restoreButton.enabled=true
            recordButton.enabled=true
            busy = false
        }
        else if(state==="running")
        {
            runButton.enabled=false
            speedButton.enabled=false
            stepButton.enabled=false
            backtrackButton.enabled=FbsfTimeManager.historySize()-1 >= 1 ? true : false
            pauseButton.enabled=true
            saveButton.enabled=false
            restoreButton.enabled=false
            recordButton.enabled=false
            busy = true

        }
        else if(state==="stepping")
        {
            busy = true
        }
        else if (state==="stepEnd" )
        {
            nbSteps.decrement()
            busy = false
        }
        else if (state === "computing")
        {
            busy = true
        }

        else if (state==="waiting")
        {
            busy = false
        }

        // propagate mode and state to plugins
        for (var p=0;p<pluginsList.count;p++)
            if (typeof pluginsList.get(p).plugin.statusChanged === "function")
                pluginsList.get(p).plugin.statusChanged(mode,state)
    }

    XmlListModel{id: projectPlugins
        source: FBSF_CONFIG
        query: "/Items/simulation"
        XmlRole { name: "timestep"; query: "timestep/string()" }
        XmlRole { name: "pastsize"; query: "pastsize/string()" }
        XmlRole { name: "futursize"; query: "futursize/string()" }
        XmlRole { name: "timeshift"; query: "timeshift/string()" }

        XmlRole { name: "hidePauseBtn"; query: "hidePauseBtn/string()" }
        XmlRole { name: "hideStepBtn"; query: "hideStepBtn/string()" }
        XmlRole { name: "hideRunBtn"; query: "hideRunBtn/string()" }
        XmlRole { name: "hideSpeedBtn"; query: "hideSpeedBtn/string()" }
        XmlRole { name: "hideStepCrtBtn"; query: "hideStepCrtBtn/string()" }
        XmlRole { name: "hideNavigationBtn"; query: "hideNavigationBtn/string()" }
        XmlRole { name: "hideSnapControlBtn"; query: "hideSnapControlBtn/string()" }
        XmlRole { name: "hideBacktrackBtn"; query: "hideBacktrackBtn/string()" }

        XmlRole { name: "intialimage"; query: "intialimage/string()" }

        onStatusChanged:
        {
            switch (status)
            {
                case XmlListModel.Ready :
                    timeStep = get(0).timestep;
                    timeShift = get(0).timeshift;
                    pastSize = get(0).pastsize;
                    futurSize = get(0).futursize;
                    hidePauseBtn = (get(0).hidePauseBtn === "" || get(0).hidePauseBtn === "false") ? false : true
                    hideStepBtn = (get(0).hideStepBtn === "" || get(0).hideStepBtn === "false") ? false : true
                    hideRunBtn = (get(0).hideRunBtn === "" || get(0).hideRunBtn === "false") ? false : true
                    hideSpeedBtn = (get(0).hideSpeedBtn === "" || get(0).hideSpeedBtn === "false") ? false : true
                    hideStepCrtBtn = (get(0).hideStepCrtBtn === "" || get(0).hideStepCrtBtn === "false") ? false : true
                    hideSnapControlBtn = (get(0).hideSnapControlBtn === "" || get(0).hideSnapControlBtn === "false") ? false : true
                    hideNavigationBtn = (get(0).hideNavigationBtn === "" || get(0).hideNavigationBtn === "false") ? false : true
                    hideBacktrackBtn = (get(0).hideBacktrackBtn === "" || get(0).hideBacktrackBtn === "false") ? false : true
                    if(get(0).intialimage !== "")
                        displayVisu(get(0).intialimage)

                    break;
                case XmlListModel.Null :
                case XmlListModel.Error :
                    console.log(errorString ());
                    break;
            }
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Time subscription for text display
    // Simulation.Time : not concerned by the bactrackmode
    // Data.Time : replayed with bactrack
    // Both are vectors in case simuMpc and Scalars otherwise
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Data.Time is replayed with bactrack
    // Data.Time is vector in case simuMpc and Scalars otherwise
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SubscribeInt{id: dataTimeScalar;
        tag1 : simuMpc ? "" : "Data.Time"
        onValueChanged: {
             backtrackTime.text = FbsfTimeManager.dateTimeStr(value)
        }
    }
    SubscribeVectorInt{id: dataTimeVector;
        tag1 : simuMpc ? "Data.Time" : ""
        onDataChanged  : {
            backtrackTime.text = FbsfTimeManager.dateTimeStr(data[pastSize-1])
        }
    }
    RowLayout{
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10
        TBButton{id: help
            source:"icons/button_help.png";
            onClicked: {
                Qt.openUrlExternally(FBSF_HOME+"/Documentation/pdf/MutFrameworkToolBar.pdf")
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Simulation Time
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        Text{id: simulationTime
            text:FbsfTimeManager.DateTimeStr
            font.pixelSize: 20
            color: "green"
            horizontalAlignment: Text.AlignHCenter
            Rectangle {
                anchors.fill: parent
                anchors.margins: -4
                color: busy ? "#99FFFFFF" : "transparent"
                border.width: 2
                border.color: "green"
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Simulation control
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        RowLayout{id: simControl
            TBButton{id: pauseButton
                visible:!hidePauseBtn
                enabled : false
                source : "icons/button_pause.png"  ;
                label : qsTr("Pause simulation")
                onClicked: {framework.executiveControl("pause");}
            }
            TBButton{id: stepButton
                visible:!hideStepBtn
                enabled : true
                source : "icons/button_step.png"  ;
                label : qsTr("Step")
                onClicked:
                {
                    framework.executiveControl("step")
                    recordButton.enabled=true
                }
            }
            TBButton{id: runButton
                visible:!hideRunBtn
                source : "icons/button_run.png"  ;
                label : qsTr("Run")
                onClicked: {
                    if(nbSteps.text!=="") pauseButton.enabled=true

                    if(stepToExit.selected)
                        framework.executiveControl("run",nbSteps.text,"stop")
                    else
                        framework.executiveControl("run",nbSteps.text,"pause")
                    nbSteps.focus = false
                }
            }
            TBButton{id: speedButton
                visible: !hideSpeedBtn
                enabled : true
                source : "icons/button_speed.png"  ;
                label : qsTr("Speed")
                onClicked: {
                    if(nbSteps.text!=="") pauseButton.enabled=true

                    if(stepToExit.selected)
                        framework.executiveControl("speed",nbSteps.text,"stop")
                    else
                        framework.executiveControl("speed",nbSteps.text,"pause")
                    nbSteps.focus = false
                }
            }
            TextField{id: nbSteps
                visible:!hideStepCrtBtn
                implicitWidth           : 50
                font.pixelSize          : 12
                font.bold               : true
                selectByMouse           : true
                text                    : ""
                enabled                 : runButton.enabled
                opacity                 : enabled ? 1 : .4
                validator               : IntValidator {bottom:0;}
                horizontalAlignment     : TextInput.AlignRight
                style: TextFieldStyle {
                    textColor       : "green"
                    background      : Rectangle {
                        color       : "lightgrey"
                        radius      : height/4
                        border.color: "lightgrey"
                        border.width: 1
                    }
                }
                function decrement()
                {
                    if(text==="") return // infinite number of steps
                    var num=parseInt(text)
                    if(num>1)
                        text=--num
                    else
                        text=""
                }
            }
            ColumnLayout{
                spacing: -5
                TBButton{id: stepToPause
                    visible:!hideStepCrtBtn
                    property bool selected : true
                    source : "icons/button_pause.png"
                    scale: 0.7
                    highlighted             : selected
                    enabled                 : stepButton.enabled
                    onClicked               : {stepToExit.selected=false;selected=true}
                }
                TBButton{id: stepToExit
                    visible:!hideStepCrtBtn
                    property bool selected : false
                    source  : "icons/button_exit.png"
                    scale: 0.7
                    highlighted             : selected
                    enabled                 : stepButton.enabled
                    onClicked               : {stepToPause.selected=false;selected=true}
                }
            }
            TBButton{id: warningButton
                visible : false
                source : "icons/button_warning.png"  ;
                label : qsTr("Warning ")
                onClicked: {Qt.openUrlExternally(CURRENT_DIR+"/FbsfFramework.log");}
            }
        }

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Snapshot control
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        RowLayout{id: snapControl
            TBButton{id: saveButton
                visible: !hideSnapControlBtn
                enabled : true
                source : "icons/button_save.png"  ;
                label : qsTr("Save states")
                onClicked: {
                    restoreButton.enabled=true
                    fileDialog.nameFilters=["Snapshot files (*.snp)"]
                    fileDialog.title="SaveStates"
                    fileDialog.selectExisting=false
                    fileDialog.open()
                }
            }
            TBButton{id: restoreButton
                visible: !hideSnapControlBtn
                enabled : true
                source : "icons/button_restore.png"  ;
                label : qsTr("Restore states")
                onClicked: {
                    restoreButton.enabled=true
                    fileDialog.nameFilters=["Snapshot files (*.snp)"]
                    fileDialog.title="RestoreStates"
                    fileDialog.selectExisting=true
                    fileDialog.open()
                }
            }
            TBButton{id: recordButton
                visible: !hideSnapControlBtn
                enabled : false
                source : "icons/button_record.png"  ;
                label : qsTr("Record simulation")
                onClicked: {
                    fileDialog.nameFilters=["Data files (*.dat)"]
                    fileDialog.title="Record"
                    fileDialog.selectExisting=false
                    fileDialog.open()
                }
            }
        }

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Plugin Navigation
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        ComboBox{id:pluginCb
            visible: !hideNavigationBtn
            textRole: "name"
            implicitWidth: 200
            model: pluginsList
            ListModel {id: pluginsList}

            onCurrentIndexChanged: {
                for (var i = 0; i < pluginsList.count; i++)
                {
                    if (i === currentIndex)
                        pluginsList.get(i).plugin.visible = true
                    else
                        pluginsList.get(i).plugin.visible = false
                }
            }
            style: ComboBoxStyle {
                background: Rectangle {
                    id: rectCategory
                    radius: 5
                    border.width: 2
                    color: "#fff"
                }
                label: Text {
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 12
                    color: "black"
                    text: control.currentText
                }
            }
        }


        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Backtrack and replay interface
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        RowLayout{
            visible: pluginsList.get(pluginCb.currentIndex)!==undefined?
                          // Backtrak visible for graphical qml images supervision
                         pluginsList.get(pluginCb.currentIndex).backtrackable:true
            TBButton{id: backtrackButton
                visible: !hideBacktrackBtn
                source : "icons/button_time.png";
                label : qsTr("Backtrack")
                onClicked: {
                    if (backtrackSlider.visible == true) {
                        framework.executiveControl("backtrack", "off");
                    } else {
                        if (simuMpc) {
                            backtrackSlider.maximumValue=FbsfTimeManager.historyWithFuturSize()-1
                            framework.executiveControl("backtrack", "on",FbsfTimeManager.historySize());
                            backtrackSlider.value = backtrackSlider.maximumValue - futurSize
                        } else {
                            backtrackSlider.maximumValue=FbsfTimeManager.historySize()-1;
                            framework.executiveControl("backtrack", "on",backtrackSlider.maximumValue);
                            backtrackSlider.value = backtrackSlider.maximumValue;
                        }
                    }
                    backtrackSlider.visible=!backtrackSlider.visible
                    backtrackRun.enabled = true
                }
            }

            // Slider for replay
            Slider {id:replaySlider
                height                      : parent.height * 0.8
                visible                     : false
                minimumValue                : 0
                value                       : FbsfDataModel.replaySteps
                onPressedChanged:{
                    if(pressed==false)
                        framework.executiveControl("replay",value.toString());
                }
                stepSize        : 1
                style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 600
                        implicitHeight: 4
                        color: "gray"
                        radius: 8
                    }
                    handle: Rectangle {
                        anchors.centerIn: parent
                        color: control.pressed ? "white" : "lightgray"
                        border.color: "gray"
                        border.width: 2
                        implicitWidth: 24
                        implicitHeight: 24
                        radius: 8
                    }
                }
            }
            // Slider for backtrack
            Slider {id:backtrackSlider
                visible: false
                implicitWidth: 300
                implicitHeight: 30
                minimumValue: 0
                value: maximumValue
                tickmarksEnabled: true
                onValueChanged: {
                    framework.executiveControl("backtrack", value.toString());
                }
                stepSize        : 1
                style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 4
                        color: "gray"
                        radius: 8
                    }
                    handle: Rectangle {
                        anchors.centerIn: parent
                        color: control.pressed ? "lightblue" : "blue"
                        border.color: "gray"
                        border.width: 2
                        implicitWidth: 15
                        implicitHeight: 20
                        radius: 4
                    }
                    tickmarks: Repeater {
                        id: repeater
                        model: backtrackSlider.stepSize > 0 ? 1 + (backtrackSlider.maximumValue - backtrackSlider.minimumValue) / backtrackSlider.stepSize : 0
                        Rectangle {
                            visible: index === (backtrackSlider.maximumValue - futurSize + 1)
                            anchors.verticalCenter: parent.verticalCenter
                            color: "red"
                            width: 2
                            height: 40
                            x: (styleData.handleWidth / 2 + (index) * ((repeater.width - styleData.handleWidth) / (repeater.count-1))) - (styleData.handleWidth / 2 + index * ((repeater.width - styleData.handleWidth) / (repeater.count-1))) / index / 2
                        }
                    }
                }
            }
            // Slider replay time min and max
            Text {id : sliderReplayMax
                visible : replaySlider.visible
                color : "white"
            }

            // Slider backtrack value
            Text {id : backtrackTime
                visible : backtrackSlider.visible
                color:"blue"
                y:10;
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: -4
                    color: "transparent"
                    border.width: 2
                    border.color: "blue"
                }
            }
            ToolButton{id: backtrackPause
                visible: backtrackSlider.visible
                enabled: !backtrackRun.enabled
                iconSource : "icons/button_pause.png";
                tooltip : qsTr("Pause backtrack")
                onClicked: {
                    backtrackRun.enabled=true;
                }
            }
            ToolButton{id: backtrackStep
                visible: backtrackSlider.visible
                enabled: backtrackRun.enabled
                iconSource : "icons/button_step.png";
                tooltip : qsTr("Step backtrack")
                onClicked: {
                    if (backtrackSlider.value < backtrackSlider.maximumValue) {
                        backtrackSlider.value += 1;
                    } else {
                        backtrackSlider.value = 0;
                    }
                }
            }
            ToolButton{id:backtrackRun
                visible: backtrackSlider.visible
                iconSource : "icons/button_run.png";
                tooltip : qsTr("Run backtrack")
                onClicked: {
                    backtrackRun.enabled=false;
                    runBacktrack(parseInt(backtrackNbTimeStep.text), parseFloat(1/fps.text), 0, backtrackSlider.value);
                }
            }
            // Number of steps to execute in backtrack
            TextField {id: backtrackNbTimeStep
                visible: backtrackSlider.visible
                implicitWidth: 50
                font.pixelSize: 12
                font.bold: true
                selectByMouse: true
                text: backtrackSlider.maximumValue
                enabled: backtrackRun.enabled
                opacity: enabled ? 1 : .4
                validator: IntValidator {bottom:0;}
                horizontalAlignment: TextInput.AlignRight
                style: TextFieldStyle {
                    textColor       : "blue"
                    background      : Rectangle {
                        color       : "lightgrey"
                        radius      : height/4
                        border.color: "lightgrey"
                        border.width: 1
                    }
                }
                Tooltip{text:"Nb of steps used in run mode";anchors.top:parent.bottom;display:backtrackNbTimeStep.hovered}
            }
            // Speed of defilement in backtrack
            TextField {id: fps
                visible: backtrackSlider.visible
                implicitWidth: 50
                font.pixelSize: 12
                font.bold: true
                selectByMouse: true
                text: "1"
                enabled: backtrackRun.enabled
                opacity: enabled ? 1 : .4
                horizontalAlignment: TextInput.AlignRight
                validator: IntValidator {bottom:1;}
                style: TextFieldStyle {
                    textColor       : "blue"
                    background      : Rectangle {
                        color       : "lightgrey"
                        radius      : height/4
                        border.color: "lightgrey"
                        border.width: 1
                    }
                }                
                Tooltip{id: tlpFps;text:"Frame Per Second in run mode";anchors.top:parent.bottom;}
                onHoveredChanged:  tlpFps.showDuring(1000)
            }
            CheckBox{id :backtrackCheckBox
                visible: backtrackSlider.visible
                checked: false
                tooltip : qsTr("Check the box to activate infinite loop in run mode")
            }
        }
    }

    function displayVisu(name)
    {
        var visuIndex = pluginCb.find(name)
        if(visuIndex !== -1)
                pluginCb.currentIndex = visuIndex
        else
            console.log ("AppToolbar error ", name,"is not a valid mdoule name to display")
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Plugin loader
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function load(name,path,configuration, visibility)
    {
        // path relative to local file
        var component = Qt.createComponent(path+"/main.qml")
        switch (component.status)
        {
            case Component.Ready :
                var plugin=component.createObject(pluginframework,{"objectName":name, "path":path,
                                                      "configuration":configuration, "visibility":visibility, "moduleName":name});
                if (plugin === null)
                    console.log("Error creating object",name);
                else if (visibility === true)
                {
                    pluginsList.append({"name":name,"plugin":plugin,"backtrackable":plugin.backtrackable})
                    if (first)
                    {
                        plugin.visible = true
                        first = false
                    }
                }

            break;
            case Component.Null :
            case Component.Error : console.log(component.errorString ());break;
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // File chooser
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    FileDialog{id: fileDialog
        onAccepted:
        {
            if (title=="SaveStates") // save a state file
            {
                framework.executiveControl("save",fileDialog.fileUrl);
            }
            else if (title=="RestoreStates") // restore a state file
            {
                framework.executiveControl("restore",fileDialog.fileUrl);
            }
            else if (title=="Record") // save a replay file
            {
                framework.executiveControl("record",fileDialog.fileUrl);
            }
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Run backtrack
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Timer {id: timer
        function setTimeout(callback, delayTime) {
            timer.interval = delayTime;
            timer.repeat = false;
            timer.triggered.connect(callback);
            timer.triggered.connect(function release () {
                timer.triggered.disconnect(callback);
                timer.triggered.disconnect(release);
            });
            timer.start();
        }
    }

    function runBacktrack(step, speed, actualStep, start)
    {
        if (backtrackRun.enabled == false)
        {
            if (actualStep >= step)
            {
                if (backtrackCheckBox.checked == true)
                {
                    backtrackSlider.value = start;
                    actualStep = 0;
                }
                else
                {
                    backtrackRun.enabled = true;
                    return;
                }
            }
            else
            {
                actualStep += 1;
                if (backtrackSlider.value < backtrackSlider.maximumValue) {
                    backtrackSlider.value += 1;
                }
                else
                {
                    backtrackSlider.value = 0;
                }
            }
            timer.setTimeout(function () {
                runBacktrack(step, speed, actualStep, start);
            }, speed * 1000);
        }
    }
}
