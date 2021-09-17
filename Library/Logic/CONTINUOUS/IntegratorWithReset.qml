import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSISO
{
    nodeType: "IntegratorWithReset"
    source  : nodeType+".svg"

    socketsTop: [
        Socket {id: resetSignal;socketId:"resetSignal";direction: "input";type: "bool"}
                ]

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yNew: internal value for y
    property var variables : {'yNew' : 0}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set state variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yOld: state value for y
    // initialise and defined during mFirstStep

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yStart : start value during at initComput
    // k : integrator gain
    property var parameters : {"yStart" :0, "k" : 1 }

    // Dynamic Display of the parameter in the document
    Text{
        id:parameterValue;
        anchors.horizontalCenter: parent.horizontalCenter;anchors.top: parent.bottom;
        anchors.topMargin: 10;
        font.pointSize:12;color: "red";horizontalAlignment : Text.AlignVCenter;
    }

    function updateDisplay()
    {
        parameterValue.text = "k = " + parameters.k;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
        updateDisplay();
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
             statesvar.mFirstStep = false;
             statesvar.yOld = parameters.yStart;
         }

        if (resetSignal.value === true) {
            variables.yNew = parameters.yStart;
        }
        else{
             variables.yNew = statesvar.yOld + timeStep * input.value * parameters.k;
        }


        //console.log(statesvar.yInternal);
        //console.log(timestep.value);
        //socketsRight[0].value = yInternal;
        //console.log("Valeur du temps de simulation ", simulationTime, " valeur du output ", output.value);
        output.value = variables.yNew;
        statesvar.yOld = variables.yNew;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{updateDisplay()}
}

