//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Object : Plugin main component
//
//  Public input properties :
//
//          appToolbar (Application toolbar to register plugin)
//          path (plugin absolute path )
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
import QtQuick 2.0

Rectangle {
    // public properties
    property variant appToolbar // Application toolbar
    property string path
    property string name: "UIManual"
    property bool  backtrackable:true

    // Graphic attributs
    anchors.fill : parent
    visible : false
    signal statusChanged(var mode,var state)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onStatusChanged:testeur.statusChanged(mode,state)
    // plugin User Interface
    UIManual{id : testeur}
    // Plugin registration to Application toolbar
//    Component.onCompleted:
//    {
//        appToolbar.appIcons.append({"name":name,"pluginIcon":path+"/"+name+".png"})
//    }
}
