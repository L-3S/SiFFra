import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Grapher 1.0
import fbsfplugins 1.0
import "../PARTIAL"

// STATES INTEGRATED

PartialSISO
{
    nodeType: "PID"
    source  : nodeType+".png"

    // Documentation
    // y = 0.5 * (yMax + yMin) +
    //     k * (u + sum(u/Tint*dt) + ((unew-uold)/dt*Tder))

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // yInternal: internal value for y
    // proportioTerm: proportional term
    // integralTerm: integral term
    // derivativeTerm: derivative term
    // uOld : old value of input
    // uNew : new value of input

    property var variables : {'yInternal' : 0,
    'constantTerm' : 0, 'proportioTerm' : 0, 'derivativeTerm' : 0,
    'uNew' : 0}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // k : PID gain
    // tInt: time constant for Integral term
    // tDer: time constant for Derivative term
    // maxY : upper bound for Output
    // minY : lower bound for Output
    // yReInit : new value of y (by tunning of integral term)
    property var parameters : {"k" : 1,
                               "tInt" : 100, "tDer" : 0.1,
                               "maxY" : 1, "minY" : 0, "yStart" : 0.5,
                               "constantON" : true,
                               "proporON" : true,
                               "integralON" : true,
                               "derivativeON" : true}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set dependant parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // maxInt: upper bound for Integral term
    // minInt: lower bound for Integral term
    property var dependPara : {"maxInt" :0, "minInt" : 1}


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Check parameters consistency
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function checkParaConsistency()
    {
        // TESTING AND COMPUTING TO DO ONLY AT INIT OR
        // AFTER PARAMETER CHANGE BY USER IN EDITOR
        // Consistence des paramètres d'entrée
        if (parameters.maxY - parameters.minY <= 0)
        {
            console.exception(nodeName+" object: please revise parameters to ensure maxY > minY");
            console.log(nodeName+" object: parameter maxY is automatically increased");
            parameters.maxY = parameters.minY + 1;
        }
        if (parameters.k == 0)
        {
            console.exception(nodeName+" object: please revise PID gain to ensure it is different from 0");
            console.log(nodeName+" object: parameter k is automatically revised to 1");
            parameters.k = 1;
        }
        if (parameters.tInt <= 0)
        {
            console.exception(nodeName+" object: please revise integral time constant to ensure it is strictly positive");
            console.log(nodeName+" object: parameter tInt is automatically revised to 1");
        }
        if (parameters.yReInit > parameters.maxY)
        {
            console.exception(nodeName+" object: please revise parameters to ensure yReInit <= maxY");
            console.log(nodeName+" object: parameter yReInit is automatically decreased");
            parameters.yReInit = parameters.maxY;
        }
        if (parameters.yReInit < parameters.minY)
        {
            console.exception(nodeName+" object: please revise parameters to ensure yReInit >= minY");
            console.log(nodeName+" object: parameter yReInit is automatically increased");
            parameters.yReInit = parameters.minY;
        }


    }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Compute Dependant Parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function computeDependantParameters()
    {
        // Integral term can be 70 % of Yrange
        dependPara.maxInt = 0.7 * (parameters.maxY - parameters.minY) / parameters.k
        dependPara.minInt = - dependPara.maxInt;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Compute ReInit y
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initIntegralTerm()
    {
        statesvar.integralTerm = statesvar.integralTerm + parameters.yStart -
                (variables.constantTerm +
                 variables.proportioTerm +
                 statesvar.integralTerm +
                 variables.derivativeTerm);
    }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {
        if (statesvar.mFirstStep){
            // statesvar.mFirstStep = false;

            checkParaConsistency();
            computeDependantParameters();
            //statesvar.yInternal = parameters.yStart;
            statesvar.uOld = input.value;
            statesvar.integralTerm = 0;

            // statesvar.yOld = input.value;
         }

        variables.uNew = input.value;

        // Constant term
        variables.constantTerm = 0.5*(parameters.maxY + parameters.minY);

        // Proportionnal term
        variables.proportioTerm = parameters.k * variables.uNew;

        // Integral term
        //console.log("integral term " + variables.integralTerm)
        //console.log("variable uNew " + variables.uNew)
        //console.log("variable timestep " + timestep.value)
        //console.log("variable tInt " + parameters.tInt)
        statesvar.integralTerm = statesvar.integralTerm + parameters.k * variables.uNew * timeStep / parameters.tInt ;

        // Bounding of Integral term
        if (statesvar.integralTerm > dependPara.maxInt) statesvar.integralTerm = dependPara.maxInt;
        else if (statesvar.integralTerm < dependPara.minInt) statesvar.integralTerm = dependPara.minInt;

        // Derivative term
        variables.derivativeTerm = parameters.k * (variables.uNew - statesvar.uOld) * parameters.tDer / timeStep ;

        //console.log("PID, timeStep ", timeStep);

        if (statesvar.mFirstStep){
            statesvar.mFirstStep = false;
            initIntegralTerm();
         }

        if (!parameters.constantON){
            variables.constantTerm = 0;
        }
        if (!parameters.proporON){
            variables.proportioTerm = 0;
        }
        if (!parameters.integralON){
            statesvar.integralTerm = 0;
        }
        if (!parameters.derivativeON){
            variables.derivativeTerm = 0;
        }


        // All contributions
        variables.yInternal = variables.constantTerm +
                              variables.proportioTerm +
                              statesvar.integralTerm +
                              variables.derivativeTerm;


        // Bounding of PID output
        if (variables.yInternal > parameters.maxY) variables.yInternal = parameters.maxY;
        else if (variables.yInternal < parameters.minY) variables.yInternal = parameters.minY;

        output.value = variables.yInternal;

        // Update Old variables
        statesvar.uOld = variables.uNew;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Modification of parameters by user
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    onParameterModified :{checkParaConsistency(),computeDependantParameters()}
}

