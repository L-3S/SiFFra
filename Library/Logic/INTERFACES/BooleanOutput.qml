import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

PartialBoolSI
{
    nodeType: "BooleanOutput"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // unit     : unit in ZE
    // comment  : comment for ZE
    property var parameters : {"unit" :"unit for variable in ZE", "comment" : "comment for variable in ZE" }

    Component.onCompleted: {
        scalar.unit = parameters.unit
        scalar.description = parameters.comment

        timeVector.unit = parameters.unit
        timeVector.description = parameters.comment
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Notification of a command value
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property int mScaCommand: NaN
    SignalInt{
        id          : scalar
        tag1        : (!simuMpc) ? nodeName : "";
        value       : mScaCommand
        unit        : parameters.unit
        description : parameters.comment
    }

    /// Values for SimuMPC mode
    SignalVectorInt{
        id          : timeVector
        tag1        : (simuMpc) ? nodeName : "";
        unit        : parameters.unit
        description : parameters.comment
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);

        if (!simuMpc) {
            /// Cosimulation mode
            // we consider here a dummy timeVector of 4 elements
            timeVector.timeindex=2
            timeVector.timeshift=1
            timeVector.data=new Array(4)

            // Initialisation à NaN, notamment pour le NoFutureProjection
            for (var k = 0; k < timeVector.data.length; k++)
                timeVector.data[k] = NaN
        }
        else {
            timeVector.timeindex=pastSize
            timeVector.timeshift=timeShift
            timeVector.data=new Array(pastSize+futurSize)

            // Initialisation à NaN, notamment pour le NoFutureProjection
            for (var i = 0; i < timeVector.data.length; i++)
                timeVector.data[i] = NaN
        }
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property int oldPastBegin : (simuMpc) ? 0 : 0;
    property int oldPastEnd : (simuMpc) ? pastSize - timeShift : 0;

    /// warning, below variable is not saved in a snapshot
    property bool firstCycle : true;
    property var saveOldpast : Array(oldPastEnd+timeShift);

    property real tempsDeb;
    property int index : 0;

    property var statesvar : {
        "mlocSimuTime" : 0,
    }

    function compute()
    {
        if (!simuMpc) {
            /// Cosimulation mode
            mScaCommand=(input.value) ? 1 : 0;
        }
        else {
            /// SimuMPC
            // Le simulationTime de FBSF s'incrémente de timeStep à chaque "cycle de calcul"
            // il devrait s'incrémenter de timeStep * timeshift
            // tempsDeb: intant physique correspondant au début du vecteur
            // cette grandeur est calculée à partir du simulationTime modifiée
            // L'index est calculé en fonction de la différence entre statesvar.mlocSimuTime et tempsDeb

            //console.log(nodeName+" object: temps simulé FBSF et temps simulé local", simulationTime - timeStep, statesvar.mlocSimuTime);

            tempsDeb = (simulationTime - timeStep)/timeStep * timeShift * timeStep;
            index = ((statesvar.mlocSimuTime - tempsDeb) / timeStep) % timeVector.data.length;

            //////// Detection of a new cycle in the simuMpc mode
            //////// this also works with FutureNoProj = true
            if (index == oldPastEnd) {

                if (!firstCycle) {
                    for (var l = 0; l < oldPastEnd; l++) {
                        ////////  Updating the mSaveOldPast "state" relying on a timeShift
                        saveOldpast[l] = saveOldpast[l+timeShift];
                        ////////  Writting the OldPast part of timeVector.data
                        timeVector.data[l] = saveOldpast[l];
                    }
                }

                if (firstCycle) firstCycle = false;
            }


            timeVector.data[index]=(input.value) ? 1 : 0;
            statesvar.mlocSimuTime += timeStep;

            ////////  Updating the mSaveOldPast "state"
            ///  during first passage, the loop traverses the whole vector
            ///  after first passsage, the loop traverses only the portion downstream from oldPastEnd
            if (index < oldPastEnd + timeShift) saveOldpast[index] = (input.value) ? 1 : 0;
        }
    }
}
