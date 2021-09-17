import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0


Node {
    nodeType: "PartialGeneric"


    // GENERIC CLASSE TO DEFINE COMMON PROPERTIES

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Subscription to a external value
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //property real mSimuTime: 0
    //SubscribeReal{
    //    tag1: "Simulation.Time";
    //    onValueChanged: {mSimuTime = value} }



    //property var timestep  : {'value'  :0.5}


/*    property real mTimeStep : 0.5
/*    property real mSimuTime : 0.*/

//property bool mFirstStep : true;
property var statesvar : {"mFirstStep": true}


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // get the PROJECT plugin list
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*    XmlListModel
    {
         id: tvParams
         source: FBSF_CONFIG
         query: "/Items/simulation"
         XmlRole { name: "timestep"; query: "timestep/string()" }

         onStatusChanged:
         {
             switch (status)
             {
             case XmlListModel.Ready :
                 timestep.value=parseFloat(get(0).timestep)
                 console.log("PartialGeneric lecture du fichier de config réussie - timestep = "+timestep.value);
                 break;
             case XmlListModel.Null :
                 //console.log("PartialGeneric lecture du fichier de config erronée - cas nul");
                 //console.log(timestep.value);
                 // default value
                 console.log("PartialGeneric lecture du fichier de config erronée - cas nul = "+timestep.value);
                 break;
             case XmlListModel.Error :
                 //console.log(errorString ());
                 //console.log("PartialGeneric lecture du fichier de config - cas error");
                 // default value
                 console.log("PartialGeneric lecture du fichier de config réussie - cas error = "+timestep.value);
                 break;
             }
         }
    }
*/

}
