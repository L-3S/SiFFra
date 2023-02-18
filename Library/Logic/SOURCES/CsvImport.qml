import QtQuick 2.0
import Grapher 1.0
import FbsfUtilities 1.0
import "../PARTIAL"

// STATES INTEGRATED

Node
{
    nodeType: "CsvImport"

    Text{
        id:simuTimeTxt;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.bottom; anchors.topMargin: 10;
        color: "red";horizontalAlignment : Text.AlignVCenter;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // build an output sockets array dynamically
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var ports :{"right" :0}
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var parameters : {"filename" :"EMPTY", "periodic": false};
    property var statesvar : {"mFirstStep": true,
                              "listArray" : [],
                              "tIndex" : 0,
                              "nIndex" : 0,
                              "varList" : [],
                              "mlocSimuTime" : 0
    };

    FbsfFileIO {
        id: csvFile
        source: "File.csv"
        onError: Util.messageBox("Error",msg)
    }
    Component.onCompleted:
    {
        if (parameters.filename === "EMPTY")
            simuTimeTxt.text = "Plz Configure the filename parameter";
        else
            openCsv();
    }

    function openCsv()
    {
        //var csvFilename = FBSF_HOME + parameters.filename;
        var csvFilename = APP_HOME + parameters.filename;
        simuTimeTxt.text = csvFilename;
        csvFile.openRead(csvFilename);

        var line=csvFile.readline();
        //get the variable name(s)
        statesvar.varList=line.split(';')
        for (var iVar=0;iVar < (statesvar.varList).length;iVar++)
            (statesvar.listArray)[iVar]=[]; // array of arrays

        statesvar.nIndex = 0;
        while((line=csvFile.readline())!=="")
        {
            var ValueList = line.split(';');
            for (var iCol=0;iCol < ValueList.length;iCol++)
                (statesvar.listArray)[iCol][statesvar.nIndex] = Number(ValueList[iCol]);
            statesvar.nIndex++
        }
        csvFile.close();
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // add sockets with varList
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (var i=1;i< (statesvar.varList).length;i++)
        {
            socketContainerRight.setPort((statesvar.varList)[i],"output","real",true);
        }
        //socketContainerRight.setSockets(nodeoutputs);
        simuTimeTxt.text = "Time(s)="+statesvar.mlocSimuTime.toFixed(2);
    }

    onParameterModified :{checkParaConsistency(name)}
    function checkParaConsistency(paramName)
    {
        if(paramName === "filename")
        {
            socketContainerRight.resetPorts();
            openCsv();
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
        }

        simuTimeTxt.text = "Time(s)="+statesvar.mlocSimuTime.toFixed(2);

        //  console.log(nodeName+" "+statesvar.nIndex+" "+(statesvar.listArray)[0][statesvar.nIndex-1]);

        var interpolTime;
        // tMin et tMax doivent être mis dans les states pour éviter l'évaluation à chaque compute
        var tMin, tMax;
        tMin = (statesvar.listArray)[0][0];
        tMax = (statesvar.listArray)[0][statesvar.nIndex-1];

        if (parameters.periodic == true)
            interpolTime = statesvar.mlocSimuTime%(tMax)
        else
            interpolTime = statesvar.mlocSimuTime;

        //console.log(nodeName + " " + statesvar.mlocSimuTime + " " +statesvar.mlocSimuTime%36);


        if (interpolTime < tMin){
            for (var i=1;i< (statesvar.varList).length;i++)
                socketsRight[i-1].value = (statesvar.listArray)[i][0];
        }
        else if (tMax <= interpolTime){
            for (var i=1;i< (statesvar.varList).length;i++)
                socketsRight[i-1].value = (statesvar.listArray)[i][statesvar.nIndex-1];
        }
        else {
            ///WARNING: l'ordre de la colonne des temps doit être strictement croissant
            while(!(((statesvar.listArray)[0][statesvar.tIndex] <= interpolTime) && (interpolTime < ((statesvar.listArray)[0][statesvar.tIndex+1])) ))
            {
                if ((statesvar.listArray)[0][statesvar.tIndex] <= interpolTime)
                statesvar.tIndex ++;
                else
                statesvar.tIndex --;
            }

            var ratio = (interpolTime -(statesvar.listArray)[0][statesvar.tIndex])/((statesvar.listArray)[0][statesvar.tIndex+1]-(statesvar.listArray)[0][statesvar.tIndex]);
            for (var i=1;i< (statesvar.varList).length;i++)
                socketsRight[i-1].value = ((statesvar.listArray)[i][statesvar.tIndex+1]-(statesvar.listArray)[i][statesvar.tIndex])*ratio + (statesvar.listArray)[i][statesvar.tIndex];
        }

        statesvar.mlocSimuTime += timeStep;
    }
}
