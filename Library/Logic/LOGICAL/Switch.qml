import QtQuick 2.7
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSO
{
    nodeType: "Switch"
    source  : nodeType+".svg"

    socketsLeft: [
        Socket {id: u1;socketId: "u1";direction: "input";type: "real"},
        Socket {id: u2;socketId: "u2";direction: "input";type: "bool"},
        Socket {id: u3;socketId: "u3";direction: "input";type: "real"}
    ]

    // Canvas to draw switch line
    // The switch line is updated during first step and when input u2 changes
    // TODO: force paint refresh when restorestate()
    Canvas
    {
        id: canvas
        anchors.fill: parent

        antialiasing    : true
        renderStrategy  : Canvas.Threaded

        onPaint:
        {
            var ctx = getContext("2d")
            ctx.reset();
            ctx.lineWidth = 4;
            ctx.strokeStyle = "red"
            drawLine(ctx,u2.value)
        }
    }


    function drawLine(ctx, up)
    {

        ctx.beginPath();
        ctx.moveTo(canvas.width / 2, canvas.height / 2)

        if (up){
            ctx.lineTo(0, 1/3/2*100)
        }
        else
        {
            ctx.lineTo(0, 100-1/3/2*100)
        }

        ctx.stroke()
        ctx.stroke();
        ctx.closePath();
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
            statesvar.u2Old = false;
            canvas.requestPaint();
        }

        if (u2.value)
            output.value = u1.value;
        else
            output.value = u3.value;

        // refresh dynamic view if u2.value has changed
        if (u2.value !== statesvar.u2Old)
            canvas.requestPaint();


        // update "Old" value
        statesvar.u2Old = u2.value;

    }
}
