//import QtQuick 2.0
import QtQuick.Controls 2.14
 import QtQuick 2.15
 import QtQuick.Dialogs 1.3
ScrollView {
    id: inspector
    y:50
    x:50
    width: popupinspect.width
    height: popupinspect.height - y

    property var idnodee: ""
    property var savinfo: ""
    property var listinfo: []
    property var listparam: []
    property var savmap: ""
    property var savidnode: ""
    property var savallparam: []
    property var butsave: ""


    Text {
        id: componentType
        text: ""
        x: 10
//        y:100
        anchors.top: parent.top
        anchors.topMargin: 10
        font.pixelSize: 18
    }


    ListView {
           width: parent.width
           height: parent.height
            model: spaceManager.getSizeInfos(idnode) + 1
            delegate: Component {
                Loader {
                    id: squareLoader
                    height: 50
//                    source: "ConfiCaseQml.qml"
//                    setSource: squareLoader.setSource("ConfiCaseQml.qml",{ "title": "blue" })
                        Component.onCompleted: {
                            if (index === spaceManager.getSizeInfos(idnode))
                                squareLoader.setSource("ButtonSaveQml.qml", {savfunc:saveoui});
                            createByIndex(idnode, index, squareLoader);
                        }
                }
        }
    }


    function deleteold()
    {
        var y = 0;

        while (y < listinfo.length) {
            listinfo[y].destroy();
            y += 1;
        }
        if (butsave !== "")
            butsave.destroy();
        butsave = "";
        listinfo = [];
        listparam = [];
        savmap = "";
        savidnode = ""
        butsave = ""

    }


    function dysplayWithType(parent, args, squareLoader)
    {
        var type = args[0];
        var warn = "";
        var strict = "";
        var listenum = [];
        var index;

        switch (type) {
        case "str":
            squareLoader.setSource("ConfiCaseQml.qml",{ "title": args[1], textt: args[2]});
            break;
        case "dbl":
            type = "dbl";

            if (args[3] !== undefined && args[4] !== undefined) {
                strict = args[3] + "-" + args[4];
                squareLoader.setSource("DblInput.qml",{ title: args[1], textt: args[2], limite:strict, min:args[3], max:args[4]});
            }
            else {
                squareLoader.setSource("DblInput.qml",{ title: args[1], textt: args[2], limite:strict});
            }
            if (args[5] !== undefined && args[6] !== undefined) {
                warn = args[6] + "-" + args[6];
            }

//            if (args[3] !== undefined && args[4] !== undefined) {
//                strict = args[3] + "-" + args[4];
//            }

//            if (args[5] !== undefined && args[6] !== undefined) {
//                warn = args[6] + "-" + args[6];
//            }
////            parent = createNbConfigCase(parent, args[1], args[2], "DblInput.qml", args)
//            squareLoader.setSource("DblInput.qml",{ title: args[1], textt: args[2], limite:strict});

            break;
        case "int":
            type = "int";
            if (args[3] !== undefined && args[4] !== undefined) {
                strict = args[3] + "-" + args[4];
                squareLoader.setSource("IntInput.qml",{ title: args[1], textt: args[2], limite:strict, min:args[3], max:args[4]});
            }
            else {
                squareLoader.setSource("IntInput.qml",{ title: args[1], textt: args[2], limite:strict});
            }
            if (args[5] !== undefined && args[6] !== undefined) {
                warn = args[6] + "-" + args[6];
            }

            break;
        case "bool":
            type = "bool";

            listenum = ["", "True", "False"]
            index = listenum.indexOf(args[2]);
            if (index === -1)
                index = 0;
            squareLoader.setSource("EnumStrInput.qml",{ "title": args[1], textt: index, listenu:listenum});
            break;
        case "path":
            type = "path";
            squareLoader.setSource("ConfiCaseQml.qml",{ "title": args[1], textt: args[2]});
            break;
        case "date":
            type = "date";
            break;
        case "dateutc":
            type = "dateutc";
            break;
        case "enumstring":
            type = "enumstring";
            listenum = args[3].split("#")
            listenum.unshift("")
            index = listenum.indexOf(args[2]);
                    if (index === -1)
                        index = 0;
            squareLoader.setSource("EnumStrInput.qml",{ "title": args[1], textt: index, listenu:listenum});
            break;
        case "enumint":
            type = "enumint";
            listenum = args[3].split("#")
            listenum.unshift("")
            index = listenum.indexOf(args[2]);
                    if (index === -1)
                        index = 0;
            squareLoader.setSource("EnumStrInput.qml",{ "title": args[1], textt: index, listenu:listenum});
            break;
        }
        parent = squareLoader.item;
        return parent;
    }
    function createByIndex(idnode, index, squareLoader)
    {
        var tmp = spaceManager.getInfos(idnode, index)

        var parent = dysplayWithType(parent, tmp, squareLoader);
        listinfo.push(parent);
    }

    MessageDialog {
        id: valuenotinbounds
        title: "Bad Parameters"
        property var name: ""
        text: "value " + name + " not in bounds"
        onAccepted: {
//            console.log("And of course you could only agree.")
//            Qt.quit()
        }
//        Component.onCompleted: visible = true
    }

    function saveoui()
    {
        var y = 0;

        while (y < listinfo.length - 1) {
            console.log(listinfo[y].test.text);
            console.log(listinfo[y].title);
            if (listinfo[y].type === "int" || listinfo[y].type === "dbl") {
                if (listinfo[y].test.text < listinfo[y].min || listinfo[y].test.text > listinfo[y].max) {
                    valuenotinbounds.name = listinfo[y].title;
                    valuenotinbounds.open()
                    return;
                }
            }

//            spaceManager.saveInfos(listinfo[y].title, listinfo[y].test.text)
            y += 1;
        }
        y = 0;

        while (y < listinfo.length - 1) {
            console.log(listinfo[y].test.text);
            console.log(listinfo[y].title);
            if (listinfo[y].type === "int") {
                if (listinfo[y].test.text < listinfo[y].min || listinfo[y].test.text > listinfo[y].max)
                    console.log("BAD LIMITE")
                    return;
            }

            spaceManager.saveInfos(listinfo[y].title, listinfo[y].test.text)
            y += 1;
        }
    }

    function addCase(parent)
    {
        var listfreeparam = [...savallparam];
        var i = 0;
        var w = 0;


        console.log(savallparam);
        while (w < listinfo.length) {
            while (i < listfreeparam.length) {
                console.log("1 " + listinfo[w].title);
                console.log("2 " + listfreeparam[i]);
                if (listfreeparam[i] === listinfo[w].title) {
                    listfreeparam.splice(i, 1);
                    break;
                }
                i += 1;
            }
            i = 0;
            w += 1;
        }
        console.log(savallparam);
        console.log(listfreeparam);
        listfreeparam.unshift("");

        var component = Qt.createComponent("ButtonAddParamQml.qml");
        var savcomponent = component.createObject(parent, {model: listfreeparam});
        return savcomponent;

    }

    function addnew(todestroy, newelem, index)
    {
        listparam.push(newelem);
        var tmp = createConfigCase(listinfo[listinfo.length - 1], newelem, "");
        listinfo.push(tmp);
        butsave.destroy();
        butsave = addCase(tmp);
        createButtonSave(butsave);
    }

    function checkDel(idnode)
    {
        deleteold();
        //        if (idnode === savidnode) {
        //        }
    }


}
