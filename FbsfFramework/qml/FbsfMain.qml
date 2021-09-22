import QtQuick 2.1
import QtQuick.XmlListModel 2.0

import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import QtQuick.Window 2.1
import "Components"

ApplicationWindow {

    id:appFramework
    width: Screen.width
    height:Screen.height
    minimumWidth: 800
    property string executiveMode
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Signal workflow control to executive
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    signal guiControl(string state,string param1,string param2)

    function executiveControl(state,param1,param2)
    {
        if(param1===undefined) guiControl(state,"","")
        else if(param2===undefined) guiControl(state,param1,"")
        else guiControl(state,param1,param2)
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Slot for executive status changes
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function statusChanged(mode,state)
    {
        executiveMode=mode
        frameworkToolbar.statusChanged(mode,state)
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Application main toolbar
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    AppToolbar{id: frameworkToolbar
        height:60
        z:1000
        framework:appFramework
        pluginframework:pluginframework
    }
    Rectangle{id: pluginframework
        width : parent.width
        height: parent.height - frameworkToolbar.height
        anchors.top:    frameworkToolbar.bottom
        anchors.right:  parent.right
        anchors.left:   parent.left
        anchors.bottom: parent.bottom
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // get the PROJECT plugin list
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    XmlListModel{id: projectPlugins
         source: FBSF_CONFIG
         query: "/Items/PluginsList/Plugin"
         XmlRole { name: "name"; query: "name/string()" }
         XmlRole { name: "path"; query: "path/string()" }
         XmlRole { name: "document"; query: "document/string()" }

         onStatusChanged:
         {
             switch (status)
             {
             case XmlListModel.Ready :
                 for(var i=0;i< count ; i++)
                 {
                     console.info("[PluginLoader] loading",
                                 APP_HOME+get(i).name,get(i).path,get(i).document);
                     frameworkToolbar.load(get(i).name
                                           ,APP_HOME+"/"+get(i).path
                                           ,APP_HOME+"/"+get(i).document,true)
                 }
                 break;
             case XmlListModel.Null :
             case XmlListModel.Error :console.log(errorString ());
                 break;
             }
         }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // get the FBSF plugin list : current dir then fbsf home
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    XmlListModel{id: frameworkPlugins
         source: CURRENT_DIR+"/plugins.xml"
         query: "/Items/PluginsList/Plugin"
         XmlRole { name: "name"; query: "name/string()" }
         XmlRole { name: "path"; query: "path/string()" }

         onStatusChanged:
         {
             switch (status)
             {
             case XmlListModel.Ready :
                 var qrcpath="../../"
                 if (appFramework.visible===true)
                 {
                     for(var i=0;i< count ; i++)
                     {
                         console.info("[PluginLoader] loading ",get(i).name,qrcpath+get(i).path);
                         frameworkToolbar.load(get(i).name,qrcpath+get(i).path,source,true)
                     }
                 }
                 break;
             case XmlListModel.Null :
             case XmlListModel.Error :

                 console.info("Reading",FBSF_HOME+"plugins.xml")
                 source=FBSF_HOME+"plugins.xml";
                    //console.log(errorString ());

                 break;
             }
         }
    }
}
