import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

PartialBoolSISO
{
    nodeType: "FallingEdge"
    source  : nodeType+".svg"

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // preU: previous value for u
    // yInternal: internal value for y
    property var variables : {'preU' : true,'yInternal' : true}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // preUstart : initila value for u
    property var parameters : {"preUstart" :false }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
        variables.preU = parameters.preUstart;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
//        console.log("variables.preU ", variables.preU, "input.value ", input.value);
//        console.log("variables.preU ", typeof(variables.preU), "input.value ", typeof(input.value));
        variables.yInternal = false;
/*        if (variables.preU===true | variables.preU === 1 | variables.preU==="true"){
            if (input.value === false | input.value === 0 | input.value === "false"){
                variables.yInternal = true;
            }
        }*/
            if (variables.preU===true){
    //            console.log("première condition")
                if (input.value === false){
    //                console.log("deuxième condition")
                    variables.yInternal = true;
                }
        }

        output.value = variables.yInternal;

       variables.preU =  input.value;
    }

}
