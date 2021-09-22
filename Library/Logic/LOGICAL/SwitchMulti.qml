import QtQuick 2.7
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSwitchMulti
{
    nodeType: "SwitchMulti"
    //source  : nodeType+".svg"

    shape.width : 100
    shape.height : socketContainerLeft.height
    //shape.Text.visible : false

    // Pour le rectangle par défaut: shape.width et shape.height
    // Pour l'icon, sourceSize.width et sourceSize.height

    //shape.height : (socketsLeft.length-1)*100/3+20
    //sourceSize.width : 100
    //sourceSize.height : (socketsLeft.length-1)*100/3+20


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
            drawLine(ctx,socketsTop[0].value)
        }
    }


    function drawLine(ctx, up)
    {
        ctx.beginPath();
        //ctx.moveTo(canvas.width / 2, canvas.height / 2)
        ctx.moveTo(canvas.width, canvas.height / 2)
        ctx.lineTo(canvas.width / 2, canvas.height / 2)


        var indiceSocket;
        indiceSocket = socketsTop[0].value - 1;
        if (indiceSocket < 0) indiceSocket = 0
        else if (indiceSocket > socketsLeft.length - 1) indiceSocket = socketsLeft.length - 1


        //var destinationY = 1/3/2*100+indiceSocket*32;
        var destinationY = 16 + indiceSocket*32;
        ctx.lineTo(0, destinationY)

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
            statesvar.uintOld = 1;
            canvas.requestPaint();
        }



        var indiceSocket = socketsLeft.length;
        indiceSocket = socketsTop[0].value - 1;
        if (indiceSocket < 0) indiceSocket = 0
        else if (indiceSocket > socketsLeft.length - 1) indiceSocket = socketsLeft.length - 1

        output.value = socketsLeft[indiceSocket].value;


        // refresh dynamic view if uint.value has changed
        if (socketsTop[0].value !== statesvar.uintOld)
            canvas.requestPaint();


        // update "Old" value
        statesvar.uintOld = socketsTop[0].value;

    }
}
