//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Object : Plugin main component
//
//  Public input properties :
//
//
//  Signal thrown :
//
//  Signal handled :
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
import QtQuick 2.0

Rectangle {
    // public properties
    property string path
    property string name: "Monitor"
    property bool  backtrackable:false

    // Graphic attributs
    anchors.fill    : parent
    visible         : false


    signal statusChanged(var mode,var state)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onStatusChanged:plugin.statusChanged(mode,state)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Plugin qml component
    Monitor{id:plugin}
}
