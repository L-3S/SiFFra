import QtQuick 2.0
import QtQml.Models 2.12

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Item {
    id: treeItem
    property alias itemRow : itemRow
    property alias subTree : subTree
    property alias subList : subList
    property bool highlight : selected
    property bool menuEnabled : false

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setSelected(footerText)
    {
        var itemIndex=subTree.rootIndex
        currentSubTree=subTree

        // Check single selection
        if(ism.hasSelection && !keyboard.ctrlPressed && !keyboard.shiftPressed)
        {
            // Clear old selected indexes
            for (var i=ism.selectedIndexes.length-1;i>=0;i--)
            {
                if(ism.selectedIndexes[i]!==itemIndex)
                {   // Except current
                    ism.select(ism.selectedIndexes[i], ItemSelectionModel.Deselect)
                }
            }
        }
        if(!keyboard.shiftPressed) // Toggle the current selection
            ism.select(itemIndex, ItemSelectionModel.Toggle)

        if(keyboard.shiftPressed)
        {
            // Extend from current to selected
            var start,end
            if(ism.currentIndex.row>itemIndex.row)
            {start=itemIndex.row;end=ism.currentIndex.row}
            else
            {start=ism.currentIndex.row;end=itemIndex.row}
            for (var r=start;r<=end;r++)
            {
                ism.select(subTree.model.index(r, 0,itemIndex.parent),
                           ItemSelectionModel.Select)
            }
        }
        // Disable context menu when multiple selection
        menuEnabled=(ism.selectedIndexes.length === 1)

        // update current index
        if(ism.selectedIndexes.length === 1)
        {// Case single selection
            ism.setCurrentIndex(ism.selectedIndexes[0],
                                ItemSelectionModel.NoUpdate)
            rootApp.setFooterText(footerText)
        }
        else
        {// Case empty or multiple selection
            ism.clearCurrentIndex()
        }
        grapher.itemSelected() // emit signal to parent
    }
    //~~~~~~~~~~~~~~~~~~~ Item inspector ~~~~~~~~~~~~~~~~~~~~~~~~~
    Loader{
        id:itemInspector;
        property var    closeCallBack
        property var    itemParent        // item parent
        property var    itemName          // item instance name
        property var    itemType          // item type (module,plugin)
        property var    itemCategory      // item category
        property var    itemModuleType    // item moduleType
        property var    itemParamsModel   // item params model
    }
    function showInspector()
    {
        itemInspector.itemParent=treeItem       // item instance
        itemInspector.itemName=name             // item name
        itemInspector.itemType=type             // item type
        itemInspector.itemCategory=category     // item category
        itemInspector.itemModuleType=moduleType // item moduleType
        itemInspector.itemParamsModel=params    // item params model
        itemInspector.closeCallBack = resetSource
        itemInspector.source="../Inspectors/ItemInspector.qml"
    }
    function resetSource() {
        itemInspector.source=""
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setName(text)
    {
        return controller.setItemName(subTree.rootIndex,text)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function setModuleType(text)
    {
        controller.setModuleType(subTree.rootIndex,text)
        itemInspector.item.updatedModel=params
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Row {
        id      : itemRow
        width   : subList.width
        height  : subList.height

        //~~~~~~~~~~~~~ Item Sublist ~~~~~~~~~~~~~~~~~~~~~~~~~~
        ListView {
            id          : subList
            objectName  : "default"
            spacing     :  orientation=== ListView.Vertical?vSpacing:hSpacing
            implicitWidth : contentItem.childrenRect.width
            implicitHeight: contentItem.childrenRect.height
            model       : subTree
            orientation : type==="fork"?ListView.Vertical:ListView.Horizontal
            highlightFollowsCurrentItem :false
            interactive : false
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Delegate model for subtree
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        DelegateModel
        {   id:subTree
            delegate:  Component {
                Loader {
                    id : shapeLoader
                    property var parentListview : subList
                    property var parentRow      : itemRow
                    property var parentModel    : subTree
                    source: switch(type) {
                            case "config"   : return "ConfigItem.qml"
                            case "module"   : return "ModuleItem.qml"
                            case "sequence" : return "SequenceItem.qml"
                            case "fork"     : return "ForkItem.qml"
                            case "plugins"  : return "PluginList.qml"
                            default : console.log(type,"unmanaged type")
                            }
                }
            }
            Component.onCompleted:
            {
                subList.objectName="ListOf"+name    // item's listview
                // set the rootIndex for subTree
                subTree.rootIndex = parentListview.model.modelIndex(index)
                currentSubTree=subTree
                model=treeModel
            }
        }// DelegateModel
    }// Row
}// Component
