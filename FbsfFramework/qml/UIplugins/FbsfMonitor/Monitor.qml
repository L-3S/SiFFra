import QtQuick 2.2
import fbsfplugins 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "qrc:/qml/Components"
Rectangle
{   id : root
    anchors.fill : parent
    SystemPalette {id: syspal}
    color: syspal.window

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function statusChanged(mode,state)
    {
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setFilter(role,filter)
    {
        tableView.model.filterRole=role
        tableView.model.filterCaseSensitivity= Qt.CaseInsensitive
        tableView.model.filterSyntax=FbsfFilterProxyModel.Wildcard
        tableView.model.filterString=filter
        tableView.model.setFilterWildcard(filter)
    }
    // Text for filter
    Row{
        x:15
        visible: true
        TBButton{id: help
            source:"../../../qml/Components/icons/button_help.png";
            onClicked: {
                Qt.openUrlExternally(FBSF_HOME+"/Documentation/pdf/MutMonitor.pdf")
            }
        }
        ComboBox
        {
            id: producerFilter;width:200
            editable: false
            model:FbsfDataModel.producers
            onCurrentIndexChanged:
            {
                if (currentIndex==0)
                    {currentIndex=-1;setFilter("producer","")}
                else
                    setFilter("producer",currentText)
                nameFilter.text="";unitFilter.text="";descriptionFilter.text=""
            }
        }
        TextField {id: nameFilter;width:500;font.pixelSize : 16
            placeholderText: "Name..";    onAccepted: {setFilter("name",text)
                                            unitFilter.text="";descriptionFilter.text=""}
        }
        TextField {id: unitFilter;width:100;font.pixelSize : 16
            placeholderText: "Unit..";    onAccepted: {setFilter("unit",text);
                                            nameFilter.text="";descriptionFilter.text=""}
        }
        TextField {id: descriptionFilter
            width:tableView.viewport.width-800;font.pixelSize : 16
            placeholderText: "Description.."; onAccepted: {setFilter("description",text)
                                                nameFilter.text="";unitFilter.text=""}
        }
    }
    TableView
    {   id : tableView
        model: FbsfFilterProxyModel{id: proxyModel}
        frameVisible: true
        headerVisible: true
        backgroundVisible: false
        alternatingRowColors: false
        sortIndicatorVisible: false
        anchors.margins: 15
        anchors.fill: parent
        anchors.topMargin: nameFilter.height

        property bool unresolved:false;

        //since QtQuick.Controls 1.2
        //Component.onCompleted: resizeColumnsToContents   

        TableViewColumn {role: "expand";     title: "";         width: 25;movable:false}
        TableViewColumn {role: "producer";   title: "Producer"; width: 200;movable:false; resizable : true}
        TableViewColumn {role: "name";       title: "name";     width: 350;movable:false; resizable : true}
        TableViewColumn {role: "value";      title: "value";    width: 150;movable:false; resizable : true}
        TableViewColumn {role: "unit";       title: "Unit";     width: 150;movable:false; resizable : true}
        TableViewColumn {role: "description";title: "Description";width: tableView.viewport.width-875;movable:false}
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        headerDelegate:
            Item {height : 30;
            Rectangle{
                height :parent.height
                //border.color: "black"; radius:2
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
                Text{x:10
                    text: styleData.value;
                    font.bold: true
                    font.pixelSize : 20
                }}}
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        rowDelegate:
            Rectangle{id:tableRow
                height : 25
                border.color: "grey"
                color: styleData.selected ? "#3499e6":"darkgrey"
                border.width:FbsfDataModel.isHeadVector(proxyModel.getRow(styleData.row))?2:1
            }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        itemDelegate:
            Item{   x:10; height:25 ; anchors.leftMargin:  10

            Rectangle{  id: backGround;visible: false;anchors.fill:parent;
                        anchors.leftMargin: -10;anchors.rightMargin: 10
                        border.color: "#ee185c"; color: "lightgreen"; radius:3}
            TextInput
            {
                id: inputBox
                property int index: proxyModel.getRow(styleData.row)
                readOnly: (styleData.role !=="value"
                           || !FbsfDataModel.isUnresolved(index)
                           || FbsfDataModel.isConstant(index))
                enabled : !readOnly
                color: FbsfDataModel.isParameter(index) ?"blue":
                       FbsfDataModel.isConstant(index)  ?"teal":
                       FbsfDataModel.isUnresolved(index)? "red" :
                       FbsfDataModel.replayed(index)    ? "white"
                                                        :styleData.textColor
                text:  if (styleData.value===undefined) ""
                       else if(styleData.role === "value" && FbsfDataModel.isRealNumber(index))
                            styleData.value.toFixed(2)
                       else styleData.value

                horizontalAlignment:TextInput.AlignRight
                font.pixelSize : 16
                selectByMouse : true
                inputMethodHints :Qt.ImhPreferNumbers
                validator: DoubleValidator
                {
                    locale:"C";notation: DoubleValidator.StandardNotation}
                    onAccepted: {FbsfDataModel.value(index,text);backGround.visible=true}
                }
        }
        // Toltip for consumer display
        Tooltip{id:tooltip;text:""}

        onClicked:
        {
            var index=proxyModel.getRow(row)

            if (FbsfDataModel.isHeadVector(index))
            {   // Expand vector list
                FbsfDataModel.toggleVisible(index)
                positionViewAtRow(row,ListView.Beginning)
            }
        }
        onPressAndHold:
        {   // Display category and consumer as a tooltip
            var index=proxyModel.getRow(row)
            var className=   FbsfDataModel.isParameter(index) ?"Parameter":
                             FbsfDataModel.isConstant(index)  ?"Constant":"Variable"

            tooltip.text=className+"\nConsumers : "+FbsfDataModel.consumers(index)
            tooltip.y=(row-1)*25-flickableItem.contentY
            tooltip.fontSize=16
            tooltip.color="lightsteelblue"
            tooltip.showDuring(5000)
        }
        Component.onCompleted: resizeColumnsToContents()
    }
}
