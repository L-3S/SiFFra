import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12

// import QtQuick.Controls.Styles 2.4
//import com.myself 1.0
//import QtQuick.Particles 1.14

Item {
    id: windowSimulation
//    width: 1000; height: 600
    property var zonesimu: "";
    property var racinspector: "";
    property var racList: "";
    property var racdrag: [];
    property var canplace: false;
    property var nbtype: 0;
    property var style: "";

    property bool isstart: true;
    property var start:  {
        if (isstart)
            preload(xmlapi.getloadedxml());
        isstart = false;
    }
//    property var loadt: zonesimu.parent.parent.loadXml(zonesimu.list, zonesimu);

//    color: "grey"
//    color: style.backgroundcolor
    Style {}

    // Set the flag directly from QML
//    menuBar:TopMenu {}
    ModuleArea {}
//    Inspector {}
//    PopupInspector {}

    TypeMenu {}

    function setvisibilitydrag(bool, index)
    {
        for (var i = 0; i < racdrag.length; i++) {
            racdrag[i].visible = !bool;
        }
        canplace = bool;
        nbtype = index;
    }

    function loadlib(path)
    {
        managetype.loadLib(path);
        windowSimulation.racList.reload();
    }

    function preload(path)
    {
        console.log(path)
        console.log("path " + path);
        if (path === "" || path === undefined)
            return;
        spaceManager.deleteNodeModuleAll();
        xmlapi.load(path);
        console.log("bef " + zonesimu.list.length);
        zonesimu.parent.parent.mydestroyAll();
        console.log("after " + zonesimu.list.length);
        zonesimu.parent.parent.loadXml(zonesimu.list, zonesimu);
        windowSimulation.racList.reload();
    }
}
