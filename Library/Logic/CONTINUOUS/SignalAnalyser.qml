import QtQuick 2.0
import Grapher 1.0
import "../PARTIAL"

// STATES INTEGRATED


// Computes Mean, Max, Min and Time Integral of input signal over
// sliding time interval between (present - T) and (T)
// At init: rhe history buffer is fed with the input (meaning u(t<0) = u(t=0))
// When T is increased during the simulation, the buffer extension is fed
// with the last value of the bufer

PartialSIQO
{
    nodeType: "SignalAnalyser"
    source: nodeType+".png"


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set internal properties
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set state variables
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // uHistory: buffer memory for input value
    // initialise and defined during mFirstStep


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // set parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // T : lenght of time interval in s (default = 86400)
    property var parameters : {"T" :86400 }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // computed parameters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property int bufferSize;

    onParameterModified :{computeDependantParam()}
    function computeDependantParam()
    {
        if (parameters.T < timeStep)
        {
            console.log(nodeName+" object: please revise to ensure that T >= timeStep");
            console.log(nodeName+" object: parameter T is automatically increased to timeStep value");
            parameters.T = timeStep;
        }

        // Hopefully timeStep is strictly positive
        bufferSize = Math.round(parameters.T / timeStep);

        console.log(nodeName+" object: old buffer size " + statesvar.uHistory.length + " new buffer size " + bufferSize);

        refactorHistory(statesvar.uHistory.length,bufferSize);
    }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Refactor history
    // should be called at initialisation and when parameter T is changed
    // - manages the size of the bufferSize
    // - updates the
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function refactorHistory(oldSize,newSize)
    {
        if (oldSize === 0)
        {
            // FILLING array with input value (for initialization mode)
            while (statesvar.uHistory.length <   bufferSize)
            {(statesvar.uHistory).push(input.value)}
        }
        else if (oldSize < newSize)
        {
            // FILLING with last available element
            while ((statesvar.uHistory).length <   bufferSize)
            {
                var actualSize = statesvar.uHistory.length;
                // Update MeanValue
                statesvar.mMean = computeMeanAdd(statesvar.mMean,actualSize,statesvar.uHistory[oldSize-1],1);
                // Update SlidingIntergral
                statesvar.mSlidingIntegral +=  statesvar.uHistory[oldSize-1] * timeStep;

                (statesvar.uHistory).push(statesvar.uHistory[oldSize-1]);
            }
        }
        else if (newSize < oldSize)
        {
            // Last elements are deleted
            while (statesvar.uHistory.length >   bufferSize)
            {

                actualSize = statesvar.uHistory.length;

                // Update MeanValue
                statesvar.mMean = computeMeanAdd(statesvar.mMean,actualSize,statesvar.uHistory[actualSize-1],-1);
                // Update SlidingIntergral
                statesvar.mSlidingIntegral -=  (statesvar.uHistory)[actualSize-1] * timeStep;

                (statesvar.uHistory).pop();
            }
            // Update Max, Min
            statesvar.mMax = computeMax(statesvar.uHistory);
            statesvar.mMin = computeMin(statesvar.uHistory);

        }

    }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // ComputeMean when two time intervals are added
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function computeMeanAdd(MOY1,dT1,MOY2,dT2)
    {
        var MOY;
        // combinaison de deux moyennes glissantes sur des intervalles de temps disjoints
        MOY = (MOY1 * dT1 + MOY2 * dT2)/(dT1 + dT2);
        return MOY;
    }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Compute the Max value of an array
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function computeMax(ArrayOfReal)
    {
        var MaxVal = -1e38;

        for  (var i=0;i < (ArrayOfReal.length);i++)
        {
            MaxVal = (ArrayOfReal[i] > MaxVal) ? ArrayOfReal[i] : MaxVal;
        }
        return MaxVal;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Compute the Min value of an array
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    function computeMin(ArrayOfReal)
//    {
//        var Opposed = [];

//        for  (var i=0;i < (ArrayOfReal.length);i++)
//        {
//            Opposed.push(-ArrayOfReal[i]);
//        }

//        return -computeMax(Opposed);
//    }

    function computeMin(ArrayOfReal)
    {
        var MinVal = 1e38;

        for  (var i=0;i < (ArrayOfReal.length);i++)
        {
            MinVal = (ArrayOfReal[i] < MinVal) ? ArrayOfReal[i] : MinVal;
        }
        return MinVal;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Intialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function initialize()
    {
        console.log("Initialisation de ", nodeName);

            // Initialization of outputs
            statesvar.mMax = 0;
            statesvar.mMin = 0;
            statesvar.mMean = 0;
            statesvar.mSlidingIntegral = 0;

            statesvar.uHistory = [];

            computeDependantParam();
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Synchronous computation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function compute()
    {

        if (statesvar.mFirstStep){
             statesvar.mFirstStep = false;

            // Initialization of outputs
            statesvar.mMax = input.value;
            statesvar.mMin = input.value;
            statesvar.mMean = input.value;
            statesvar.mSlidingIntegral = input.value * parameters.T;

            statesvar.uHistory = [];

            computeDependantParam();
    }
        //console.log(nodeName,"object: dump buffer",statesvar.uHistory);

        /////////////////
       // AJOUT DU PREMIER ELEMENT
       /////////////////
       // Mise à jour du Max, Min, Mean et Sliding Integral
       statesvar.mMax = (input.value > statesvar.mMax) ? input.value : statesvar.mMax;
       statesvar.mMin = (input.value < statesvar.mMin) ? input.value : statesvar.mMin;
       statesvar.mMean = computeMeanAdd(statesvar.mMean,(statesvar.uHistory).length,input.value,1);

       statesvar.mSlidingIntegral +=  input.value * timeStep;
       // ajoute un élément au début du tableau
       (statesvar.uHistory).unshift(input.value);

       /////////////////
       // SUPPRESSION DU DERNIER ELEMENT
       /////////////////
       // Mise à jour du Max, Min, Mean et Sliding Integral
       var deletedVal = (statesvar.uHistory)[(statesvar.uHistory).length-1];

       statesvar.mMean = computeMeanAdd(statesvar.mMean,(statesvar.uHistory).length,deletedVal,-1);
       statesvar.mSlidingIntegral -=  deletedVal * timeStep;
       // supprime le dernier élément du tableau
       (statesvar.uHistory).pop();

       // Prendre un peu de marge car les save/restore successifs peuvent modifier des chiffres significatifs

       statesvar.mMax = (Math.abs(deletedVal - statesvar.mMax) < 0.001) ? computeMax(statesvar.uHistory) : statesvar.mMax;
       statesvar.mMin = (Math.abs(deletedVal - statesvar.mMin) < 0.001) ? computeMin(statesvar.uHistory) : statesvar.mMin;

//       statesvar.mMax = computeMax(statesvar.uHistory);
//       statesvar.mMin = computeMin(statesvar.uHistory);

       /////////////////
       // UPDATE DES OUTPUTS
       /////////////////
       output1.value = statesvar.mMax;
       output2.value = statesvar.mMin;
       output3.value = statesvar.mMean;
       output4.value = statesvar.mSlidingIntegral;
    }
}
