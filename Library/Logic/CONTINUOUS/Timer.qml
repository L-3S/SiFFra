import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSO
{
    nodeType: "Timer"
    source  : nodeType+".svg"

    socketsLeft: [
        Socket {id: u;socketId: "u";direction: "input";type: "bool"}
    ]

    // initalTime should be stored in snapshots "State Variable"
    //property var variables : {'entryTime' : 0.,}

    // Saving imput state
    //property bool u_old:false;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
            statesvar.entryTime = 0;
            statesvar.u_old = false;
        }

        if (u.value & statesvar.u_old === false )
            statesvar.entryTime = simulationTime;

        if (u.value)
            output.value = simulationTime - statesvar.entryTime;
        else
            output.value = 0.;

        statesvar.u_old = u.value;
    }
}
