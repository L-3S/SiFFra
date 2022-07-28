#include "ModuleMini.h"

#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QRandomGenerator>
#include <chrono>
#include <thread>

using namespace std::chrono;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModuleMini::ModuleMini()
    : FBSFBaseModel()
    , mTau_Inertia(10.) // Pump inertia in multiple of timestep
    , mV_Delay (1.) // Switch ON/OFF delay in multiple of timestep
    , mSetSpeed_IN(0.)  // Unresolved input for setpoint with default value of 0
    , mSpeed_OUT(0.)    // Speed output (Zero default)
    , mSwitch_IN(0.)    // Unresolved input for switch (default 0.)
    , mState_OUT(0.)      // Pump state (default 0.)
    , mLatency(0)         // Internal counter of timesteps for switch latency
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMini::doInit()
{
    // Subscribe and publish for values
    subscribe("Pump.SetSpeed",&mSetSpeed_IN);
    subscribe("Pump.Switch",&mSwitch_IN);
    publish(QString("Pump.Speed"), &mSpeed_OUT);
    publish(QString("Pump.State"), &mState_OUT);

    return FBSF_OK;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Computation step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMini::doStep(int timeOut)
{
    clock_t start, end;

    // Switch ON/OFF strategy
    if (mSwitch_IN > 0.5 && mLatency >=1)
    {
        mState_OUT = 1.;
    }
    else if (mSwitch_IN > 0.5 && mLatency <1)
    {
        mLatency ++;
    }
    else
    {
        mLatency = 0;
        mState_OUT = 0.;
    }

    // First order
    if (mState_OUT > 0.5)
    {
        mSpeed_OUT = mSpeed_OUT*(1. - 1. / mTau_Inertia) + mSetSpeed_IN/mTau_Inertia;
    }
    else
    {
        mSpeed_OUT = mSpeed_OUT*(1. - 1. / mTau_Inertia) + mSetSpeed_IN/mTau_Inertia;
    }

    start = clock();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    end = clock();
    return ((((float) end - start)/CLOCKS_PER_SEC) > (((float)timeOut) / 1000) ? FBSF_TIMEOUT : FBSF_OK);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Finalization step
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int ModuleMini::doTerminate()
{
    // Nothing to deallocate here
    return FBSF_OK;
}
int ModuleMini::doSaveState() {
    // This is not implemented yet
    return FBSF_OK;
}

int ModuleMini::doRestoreState() {
    // THis is not implemented yes
    return FBSF_OK;
}
