import QtQuick 2.0
import Grapher 1.0
import FbsfUtilities 1.0

Node
{
    nodeType: "CsvExport"
    Text{
        id:warningTxt;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.bottom; anchors.topMargin: 10;
        color: "red";horizontalAlignment : Text.AlignVCenter;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var ports :{"left" :0}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var parameters : {"filename" :"EMPTY", "variables":"EMPTY"}
    property var varList:[];
    property var firstLine;

    FbsfFileIO {
        id: csvFile
        source: "File.csv"
        onError: Util.messageBox("Error",msg)
    }

    Component.onCompleted:
    {
        if (parameters.filename === "EMPTY" || parameters.variables === "EMPTY")
        {
            warningTxt.text = "Plz Configure the filename and Variables parameters";
            warningTxt.visible = true;
        }
        else
        {
            firstLine = "Time;" + parameters.variables;
            varList = firstLine.split(';');
            saveCsv();
        }
    }
    Component.onDestruction:{csvFile.close();}

    function saveCsv()
    {
        var csvFilename = APP_HOME +"data/" + parameters.filename;
        csvFile.open(csvFilename);

        csvFile.write(firstLine)
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // add sockets with varList
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (var i=1;i < varList.length;i++)
            socketContainerLeft.setPort(varList[i],"input","real",true);
        warningTxt.visible = false;
    }

    onParameterModified :{checkParaConsistency(name)}
    function checkParaConsistency(paramName)
    {
        socketContainerLeft.resetPorts();

        if ( parameters.filename === "EMPTY" || parameters.variables === "EMPTY")
        {
            warningTxt.text = "Plz Configure the filename and Variables parameters";
            warningTxt.visible = true;
        }
        else
        {
            if( paramName === "variables")
            {
                firstLine= "Time;" + parameters.variables;
                varList = firstLine.split(';');
            }
            saveCsv();
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        var str = simulationTime.toFixed(2);
        for (var i=0;i< varList.length-1;i++)
            str+= ";" + socketsLeft[i].value;
        csvFile.write(str);
    }
}
