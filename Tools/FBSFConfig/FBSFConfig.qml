import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import "Components"
Item{
    id : grapher
    //~~~~~~~~~~~~~~~~~~~~ interface ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var treeModel          : []
    property var currentModelIndex  : null
    property var currentSubTree     : rootModel
    property bool isCurrent         : false

    //~~~~~~~~~~~~~~~~~~~~ tunable ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property int textPixelSize  : 14

    property int hSpacing       : 20
    property int vSpacing       : 40

    property int configWidth    : 100
    property int configHeight   : 80
    property int forkWidth      : 8     // height computed
    property int moduleHeight   : 30    // width computed
    property int sequenceWidth  : 20
    property int sequenceHeight : 20

    //~~~~~~~~~~~~~~~~~~~~ internal ~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //signal          itemSelected(var modelIndex)
    property var    lastSelected : null

    width                        : (rootList.width+2*hSpacing)*zoom
    height                       : (rootList.height+2*vSpacing)*zoom

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~ Insert new child item at current selected index ~~~~~~~~~~~~~~~
    function insertChildItem()
    {
        if(currentSubTree===null) return
        controller.insertModule(currentModelIndex)
        currentSubTree.model=treeModel
    }
    //~~~~~~~~~~ remove item at current selected index ~~~~~~~~~~~~~~~~~~~~~~~~~
    function removeSelection()
    {
        if(currentModelIndex===null) return
        controller.removeSelection()
    }
    //~~~~~~~~~~ move item at previous index ~~~~~~~~~~~~~~~~~~~~~~~~~
    function moveCurrentItem(dir)
    {
        if(currentModelIndex===null) return
        controller.moveItem(currentModelIndex,dir)
        resetSelection()
    }
    //~~~~~~~~~~ fork at current selected index ~~~~~~~~~~~~~~~~~~~~~~~~~
    function forkItem()
    {
        if(currentSubTree===null) return
        if(currentModelIndex===null) return
        controller.forkItem(currentModelIndex)
        currentSubTree.model=treeModel
    }
    //~~~~~~~~~~ fork at current selected index ~~~~~~~~~~~~~~~~~~~~~~~~~
    function addPluginList()
    {
        controller.addPluginList()
    }
    //~~~~~~~~~~ Cut the current selected index ~~~~~~~~~~~~~~~~~~~~~~~~~
    function cutSelection()
    {
        controller.cutSelection()
    }
    //~~~~~~~~~~ Copy the current selected index ~~~~~~~~~~~~~~~~~~~~~~~~~
    function copySelection()
    {
        controller.copySelection()
    }
    //~~~~~~~~~~ Paste at current selected index ~~~~~~~~~~~~~~~~~~~~~~~~~
    function pasteSelection()
    {
        if(currentModelIndex===null) return
        controller.pasteSelection(currentModelIndex)
    }
    //~~~~~~~~~~ reset the cuurent selection ~~~~~~~~~~~~~~~~~~~~~~~~~
    function resetSelection()
    {
        ism.clear()
    }
    //~~~~~~~~~~ Pundo last command ~~~~~~~~~~~~~~~~~~~~~~~~~
    function undo()
    {
        resetSelection()
        controller.undo()
    }
    //~~~~~~~~~~ redo last undo ~~~~~~~~~~~~~~~~~~~~~~~~~
    function redo()
    {
        resetSelection()
        controller.redo()
    }
    //~~~~~~~~~~~~~~~~~~~~ GUI layout ~~~~~~~~~~~~~~~~~~~~

    Row {
        id          : rootSequence
        objectName  : "rootSequence"
        x           : hSpacing
        anchors.top : grapher.top
        anchors.topMargin : vSpacing
        width       : rootList.width
        height      : rootList.height
        scale       : 1

        ListView {
            id              : rootList
            anchors.top     : parent.top
            spacing         : hSpacing // visual spacing
            objectName      : "rootList"
            implicitWidth   : contentItem.childrenRect.width
            implicitHeight  : contentItem.childrenRect.height
            orientation     : ListView.Horizontal
            highlightFollowsCurrentItem :false
            interactive     : false

            model       : DelegateModel
            {
                id      : rootModel
                objectName      : "rootModel"
                delegate:
                    Component {
                    Loader {
                        property var parentListview : rootList
                        property var parentRow      : rootSequence
                        property var parentModel    : rootModel
                        source: switch(type) {
                                case "config"   : return "qrc:/Components/ConfigItem.qml"
                                case "module"   : return "qrc:/Components/ModuleItem.qml"
                                case "sequence" : return "qrc:/Components/SequenceItem.qml"
                                case "fork"     : return "qrc:/Components/ForkItem.qml"
                                case "plugins"  : return "qrc:/Components/PluginList.qml"
                                }
                    }
                }
                model   : treeModel
            }

            // Sequence base line
            Rectangle{
                id      : baseLine
                x       : 0
                y       : moduleHeight/2
                z       : -1
                width   : rootList.width
                height : 2
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~ Selection model ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ItemSelectionModel {
        id: ism
        model: controller.config
        onCurrentIndexChanged:{ currentModelIndex=currentIndex}
        onSelectedIndexesChanged: {
          controller.setSelection(selectedIndexes, selected,deselected)
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Context Menus ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Menu{
        id : configMenu

        MenuItem{text: "add plugins";   onTriggered: addPluginList()}
    }
    Menu{
        id : forkBeginMenu
        property bool pluginsEnabled

        MenuItem{text: "add sequence";  onTriggered: forkItem()}
        MenuItem{text: "remove group";  onTriggered: removeSelection()}
    }
    Menu{
        id : forkEndMenu
        MenuItem{text: "add module";    onTriggered: insertChildItem()}
    }
    Menu{
        id : sequenceMenu
        property bool downEnabled
        property bool upEnabled
        MenuItem{text: "add module";    onTriggered: insertChildItem()}
        MenuItem{text: "add group";     onTriggered: forkItem()}
        MenuItem{text: "remove sequence";onTriggered: removeSelection()}
        MenuItem{text: "move down";enabled :sequenceMenu.downEnabled;onTriggered: moveCurrentItem(1)}
        MenuItem{text: "move up";  enabled :sequenceMenu.upEnabled  ;onTriggered: moveCurrentItem(-1)}
    }
    Menu{
        id : pluginsMenu
        MenuItem{text: "add plugin";        onTriggered: insertChildItem()}
        MenuItem{text: "remove plugin list";onTriggered: removeSelection()}
    }
    Menu{
        id : moduleMenu
        property bool rightEnabled
        property bool leftEnabled
        property bool removeEnabled
        MenuItem{text: "add module";onTriggered: insertChildItem()}
        MenuItem{text: "remove module" ;enabled :moduleMenu.removeEnabled;onTriggered: removeSelection()}
        MenuItem{text: "add group";onTriggered: forkItem()}
        MenuItem{text: "move right";enabled :moduleMenu.rightEnabled;onTriggered: moveCurrentItem(1)}
        MenuItem{text: "move left"; enabled :moduleMenu.leftEnabled ;onTriggered: moveCurrentItem(-1)}
    }
    Menu{
        id : pluginMenu
        property bool rightEnabled
        property bool leftEnabled
        property bool removeEnabled
        MenuItem{text: "add plugin";onTriggered: insertChildItem()}
        MenuItem{text: "remove plugin" ;enabled :pluginMenu.removeEnabled;onTriggered: removeSelection()}
        MenuItem{text: "move right";enabled :pluginMenu.rightEnabled;onTriggered: moveCurrentItem(1)}
        MenuItem{text: "move left"; enabled :pluginMenu.leftEnabled ;onTriggered: moveCurrentItem(-1)}
    }
}// Rectangle




