//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//  Object : Plugin main component
//
//  Public input properties :
//
//          configuration ( xml formatted plugin configuration)
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
import QtQuick 2.0
import QtQuick.XmlListModel 2.0
//import QtQuick.Window 2.1
//Window {width : 1800 ; height : 1000
Rectangle {anchors.fill    : parent
    visible         : false
    property string     name          : "Plotter"
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // public properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property string     path          : ""
    property string     configuration : "" // plugin configuration

    property string     timeFormat : "s"
    property string     timeStartUTC : ""

    property bool        backtrackable:false

    signal statusChanged(var mode,var state)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onStatusChanged:plugin.statusChanged(mode,state)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // plugin User Interface
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PlotterWindow{id:plugin }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~ App configuration :  Get the time format and the start time ~~~~~~~~
    //~~~~~~~~                      Get the initial page list as xml files ~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    XmlListModel
    {
         source: FBSF_CONFIG
         query: "/Items/simulation"
         XmlRole { name: "timeformat"; query: "timeformat/string()" }
         XmlRole { name: "timestart" ; query: "timestart/string()" }
         XmlRole { name: "initialPlotList"; query: "initialPlotList/string()" }

         onStatusChanged:
         {
             switch (status)
             {
             case XmlListModel.Ready :
                     plugin.timeSettings(get(0).timeformat,get(0).timestart)
                     plugin.deserialize(APP_HOME+get(0).initialPlotList)

                 break;
             case XmlListModel.Null :
             case XmlListModel.Error :console.log(configuration,errorString ());
                 break;
             }
         }
    }
}
