import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.12

import QtQuick.Controls.Styles 1.4

Rectangle {
    id : zone
//    color: "steelblue"
    color: style.backgroundcolor

    //        anchors.fill: parent
    x: 200
    width: parent.width
    height: parent.height



    //            Image { id: image; source: "bigImage.png" }

    Rectangle {
        x: 50
        y: flick.heightcontent / 2

        color: style.colorSimulation
        radius: style.radiusmodule
        border.width: style.border
        width: style.widthModule
        height: style.widthModule

//        Button {
//            id: but
//            property var list: []
//            text: "Add Machine"
//            onClicked: {
//                var idnode = spaceManager.addChildModules();
//                createMachine(but, list, idnode);
//                //                        spaceManager.addChildModules();
//                //                        console.log(spaceManager.getPathModule())
//            }
//            Component.onCompleted:window.zonesimu = but;
//            //            Component.onCompleted:loadXml(but.list, but);
//        }
        Item {
            id:but
            property var list: []

        }

        Button {
            id: button
            text: "Edit"
//            property var list: []
            onClicked: menu.open()
            x : parent.width/2 - width/2
            y : parent.height - 45


            Menu {
                id: menu


                MenuItem {
                    text: "New Machine"
                    onTriggered: {
                        var idnode = spaceManager.addChildModules();
                        createMachine(but, but.list, idnode);
                    }
                }
//                MenuItem {
//                    text: "Open..."
//                    onTriggered: openDialog.open()
//                }
//                MenuItem {
//                    text: "Save"
//                    onTriggered: saveDialog.open()
//                }
            }
            Component.onCompleted:windowSimulation.zonesimu = but;
        }

    }
    function addNode(pare, idnode)
    {
        if (pare.list.length === 0) {
            console.log("add normally");
            var newidnode = spaceManager.addChildModules(idnode, true);
            createNode(pare, pare.list, newidnode);
        }
        else {
            console.log("add between");
            var newidnode = spaceManager.addChildModulesBetween(idnode, true, -1);
            console.log("size bef: " + pare.list[0].idnode);
            pare.list = createNodeBetween(pare, pare.list, newidnode);
            console.log("size aft: " + pare.list[0].idnode);
        }

    }


    function crtest(id, parent, list, inNode, nb)
    {
        var tmp = [];
        var y = 0;
        var newparent;


        if (nb === 0) {
            console.log("la");
            newparent = createMachine(parent, list, id);
        }
        else if (nb === 1) {
            console.log("lo");
            newparent = createSequence(parent, list, id);
        }
        else if (!spaceManager.getNodeModuleIsNode(id))
            newparent = createSpriteObjects2(parent, list, id);

        //        console.log(tmp.length);
        //        return;

        if (spaceManager.getNodeModuleIsNode(id)) {
            var node = createNode2(parent, parent.list, id);
            tmp = spaceManager.getNodeModule(id);
            console.log("cretest " + tmp.length);

            while (y < tmp.length - 1) {
                crtest(tmp[y], node, node.list, true, nb + 1); /////
                y += 1;
            }

            var endNode = createEndNode(node, [], tmp[tmp.length - 1]);
            node.endNode = endNode;

            tmp = spaceManager.getNodeModule(tmp[tmp.length - 1]);

            y = 0;
            console.log("cretest " + tmp.length);
            while (y < tmp.length) {
                crtest(tmp[y], endNode, endNode.list, false, nb + 1); /////
                y += 1;
            }

            return;
        }
        tmp = spaceManager.getNodeModule(id);


        while (y < tmp.length) {
            crtest(tmp[y], newparent, newparent.list, false, nb + 1);
            y += 1;
        }

    }


    function loadXml(list, parent) {
        console.log("id "  + spaceManager.getId());
        var id = spaceManager.getId();
        var i = 0;
        var tmp = [];
        var y = 0;

        tmp = spaceManager.getNodeModule()
        console.log(tmp.length);

        while (y < tmp.length) {
            crtest(tmp[y], parent, list, true, 0);
            y += 1;
        }
        paddingList(but.list);

    }



    function createModel(parent, list, node, name) {
        var component;
        var sprite;
        var savcomponent;

        component = Qt.createComponent(name);
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;
        list.push(savcomponent);
        paddAll();
        ajustView(list);
        return list;
    }


    function createModelBetween(parent, list, node, name) {
        var component;
        var sprite;
        var savcomponent;

        component = Qt.createComponent(name);
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;

        var i = 0;
        while (i < list.length){
            list[i].parent = savcomponent;
            i += 1;
        }
        savcomponent.list = list;
        //        list.push(savcomponent);
        list = [];
        list.push(savcomponent);
        paddAll();
        ajustView(but.list);
        return list;
    }

    function createEndNode(parent, list, node) {
        var component;
        var sprite;
        var savcomponent;

        component = Qt.createComponent("EndNodeQml.qml");
        savcomponent = component.createObject(parent, {"idnode": node});
        //        savcomponent.nb = list.length + 1;
        list.push(savcomponent);
        //        paddAll();
        //        ajustView(list);
        return savcomponent;
    }


    function createNode(parent, list, node, name) {
        var component;
        var sprite;
        var savcomponent;

        component = Qt.createComponent("NodeQml.qml");
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;


        var idEndNode = spaceManager.addChildModulesEndNode(node);


        var endNode = createEndNode(savcomponent, [], idEndNode);
        savcomponent.endNode = endNode;

        list.push(savcomponent);
        paddAll();
        ajustView(list);
        return savcomponent;
    }

    function createNodeBetween(parent, list, node, name) {
        var component;
        var sprite;
        var savcomponent;

        component = Qt.createComponent("NodeQml.qml");
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;


        var idEndNode = spaceManager.addChildModulesEndNodeBetween(node);


        var endNode = createEndNode(savcomponent, [], idEndNode);
        savcomponent.endNode = endNode;

        var i = 0;
        while (i < list.length){
            list[i].parent = savcomponent.endNode;
            i += 1;
        }


        savcomponent.endNode.list = list;
        list = [];
        list.push(savcomponent);
        paddAll();
        ajustView(list);
        return list;
    }

    function createNode2(parent, list, node) {
        var component;
        var sprite;
        var savcomponent;

        component = Qt.createComponent("NodeQml.qml");
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;


        list.push(savcomponent);
        paddAll();
        ajustView(list);
        return savcomponent;
    }

    function mydestroyAll() {
        var list = but.list;
        for (var i = 0; i < list.length; i ++) {
            console.log("DESTROYEEEEEEEEEEEEEED");
            list[i].destroy();
            list.splice(i, 1);
        }
        but.list = list;
    }


    function mydestroynext(comp, node) {
        var list = comp.parent.list;
        for (var i = 0; i < list.length; i ++) {
            if (list[i] === comp) {
                list[i].destroy();
                list.splice(i, 1);
            }
        }
        paddingList(but.list);
        ajustView(list);
        console.log("delete : " + node);
        spaceManager.deleteNodeModule(node);
    }

    function mydestroyNode(comp, node, endnode) {
        var list = comp.parent.list;
        var tmp;
        for (var i = 0; i < list.length; i ++) {
            if (list[i] === comp) {
                tmp = list[i];
                //                list[i].destroy();

                for (var y = 0; y < list[i].endNode.list.length; y ++) {
                    list.push(list[i].endNode.list[y]);
                    list[i].endNode.list[y].parent = comp.parent;
                }
                console.log(list.length);
                tmp.y += 100000;
                console.log(list.length);
                list.splice(i, 1);
                console.log(list.length);
            }
        }
        paddingList(but.list);
        //        ajustView(list);
        console.log("delete : " + node);
        spaceManager.deleteNodeModuleOnly(node);
        return list;
    }

    function mydestroy(comp, node) {
        var list = comp.parent.list;
        var tmp;
        for (var i = 0; i < list.length; i ++) {
            if (list[i] === comp) {
                tmp = list[i];
                //                list[i].destroy();

                for (var y = 0; y < list[i].list.length; y ++) {
                    list.push(list[i].list[y]);
                    list[i].list[y].parent = comp.parent;
                }
                console.log(list.length);
                tmp.y += 100000;
                console.log(list.length);
                list.splice(i, 1);
                console.log(list.length);
            }
        }
        paddingList(but.list);
        //        ajustView(list);
        console.log("delete : " + node);
        spaceManager.deleteNodeModuleOnly(node);
        return list;
    }

    function ajustView(list)
    {
        flick.heightcontent = flick.height +  110 * list.length;
        flick.contentY = 55 * list.length;

        var tmp = getMaxSizeChild(but);
        flick.contentWidth = flick.width + 110 * tmp;
        //        flick.contentX = 55 * tmp;
    }

    function getMaxSizeChild(node)
    {
        var i = 0;
        var len = node.list.length;
        var sav = 0;
        var tmpsav = 0;
        var endnode = 0;

        while (i < len) {
            tmpsav = getMaxSizeChild(node.list[i]);
            if (sav < tmpsav)
                sav = tmpsav;

            i += 1;
        }
        if (node.endNode !== "")
            endnode += 1;

        return sav + 1 + endnode;
    }

    function paddAll()
    {
        paddingList(but.list);
    }

    function paddingList(list)
    {
        var len = list.length
        var space = 130;
        var tmp = 0;
        var tab = [];
        var inc = 0;

        for (var i = 0; i < len; i ++) {
            tmp = spaceManager.getSpace(list[i].idnode)
            if (tmp === 0)
                tmp = 1;
            tab.push(tmp);
            inc += tmp;
            //            console.log(list[i].idnode)
            if (list[i].endNode !== "") {
                //                console.log("AYAYAYAAYAAAAAAAAAAAAAA");
                list[i].endNode.x = 150 * (getMaxSizeChild(list[i]) - 1);
                paddingList(list[i].endNode.list);
            }
        }
        var start = -(inc / 2 * space);
        for (var y = 0; y < list.length; y ++) {
            start = start + (tab[y]*space)/2;
            list[y].y = start;
            //                console.log("for :" + y + " pos: " + start);
            start += (tab[y] * space) / 2;
            paddingList(list[y].list);
        }
    }



    function createMachine(parent, list, node) {
        var component;
        var sprite;
        var savcomponent;

        component = Qt.createComponent("MachineQml.qml");
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;
        list.push(savcomponent);
        paddAll();

        if (sprite === null) {
            // Error Handling
            //console.log("Error creating object");
        }
        ajustView(list);
        return savcomponent;
    }


    function createSpriteObjects2(parent, list, node) {
        var component;
        var sprite;
        var savcomponent;
        //            console.log("LOLOL " + spaceManager.getSizeListModules());


        component = Qt.createComponent("ModuleQml.qml");
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;
        list.push(savcomponent);
        //paddingList(but.list);

        if (sprite === null) {
            // Error Handling
            //console.log("Error creating object");
        }

        //        flick.heightcontent += 110;
        //        flick.contentY += 55
        ajustView(list);
        return savcomponent;
    }
    function createSequence(parent, list, node) {
        var component;
        var sprite;
        var savcomponent;
        //            console.log("LOLOL " + spaceManager.getSizeListModules());


        component = Qt.createComponent("SequenceQml.qml");
        savcomponent = component.createObject(parent, {"idnode": node});
        savcomponent.nb = list.length + 1;
        list.push(savcomponent);
        paddAll();
        ajustView(list);
        return savcomponent;
    }
}
