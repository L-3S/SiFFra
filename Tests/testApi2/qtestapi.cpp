#include "testapi.h"
#include <stdio.h>
#include <QTest>
#include <string>


int TestLoadSuccess(int ac, char **av, QString& mess)
{
    FbsfStatus st = FbsfUninitialized;
    cout << "Starting Test1 - TestLoadSuccess"<<endl;

    // Instantiation
    void *pComp = FbsfInstantiate("simul.xml", ac, av);
    if (!pComp)
    {
        mess =  "ERROR: TestLoadSuccess: 1-Instantiated component is the null pointer";
        return 1;
    }

    // Obtaining the object status
    if(FbsfGetStatus(pComp, &st)==Failure || st!=FbsfReady)
    {
        mess =  "ERROR: TestLoadSuccess: 2-Instantiation step was unsuccessful";
        return 1;
    }

    // Terminate the application
    if(FbsfTerminate(pComp)==Failure)
    {
        mess =  "ERROR: TestLoadSuccess: 3-Termination step was unsuccessful";
        return 1;
    }

    // Checking the status flag update
    if(FbsfGetStatus(pComp, &st)==Failure || st!=FbsfTerminated)
    {
        mess =  "ERROR: TestLoadSuccess: 4-State after termination is not compliant";
        return 1;
    }

    // Freeing the object instance
    if(FbsfFreeInstance(&pComp)==Failure || pComp)
    {
        mess =  "ERROR: TestLoadSuccess: 5-FreeInstance step was unsuccessful";
        return 1;
    }

    return 0;
};

int TestLoadFailure(int ac, char **av, QString& mess)
{
    FbsfStatus st = FbsfUninitialized;
    cout << "Starting Test1b - TestLoadFailure"<<endl;
    void *pComp = FbsfInstantiate("simulBof.xml", ac, av);
    if(!pComp)
    {
        mess =  "ERROR: TestLoadFailure: 1-Returned pointer from allocation is the null pointer";
        return 1;
    }
    if(FbsfGetStatus(pComp, &st)==Failure || st!=FbsfUninitialized)
    {
        mess =  "ERROR: TestLoadFailure: 2-Instantiate step has wrong return flag or was unsuccessful";
        return 1;
    }
    if(FbsfTerminate(pComp)!=Failure)
    {
        mess =  "ERROR: TestLoadFailure: 3-Terminate step has wrong return flag";
        return 1;
    }
    if(FbsfFreeInstance(&pComp)==Failure || pComp)
    {
        mess =  "ERROR: TestLoadFailure: 4-FreeInstance step was unsuccessful or did not free the pointer";
        return 1;
    }
    return 0;
};

int TestSimpleSimu(int ac, char **av, QString& mess)
{
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 0.2;
    const double timeOutCPUs = 1e6;
    const QString simuTimeString = "Simulation.Time";
    double ZEValTime = -1;
    cout << "Starting Test2 - TestSimpleSimu"<<endl;

    void *pComp = FbsfInstantiate("simul.xml", ac, av);
    if(!pComp || FbsfGetStatus(pComp, &st)==Failure || st==FbsfUninitialized)
    {
        mess =  "ERROR: TestSimpleSimu: 1-Initialization was not successful";
        return 1;
    };

    if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime) > 1.e-9){
        mess =  "ERROR: TestSimpleSimu: 2-Read of initial time for simulation was unsuccessful";
        return 1;
    };

    for (int yy = 0; yy < 10; yy++){

        if(FbsfDoStep(pComp, timeOutCPUs) == Failure || FbsfGetStatus(pComp, &st)==Failure || st!=FbsfReady){
            mess =  "ERROR: TestSimpleSimu: 3-Timestepping was unsuccessful";
            return 1;
        }

        if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime - (yy+1) * timestep) > 1.e-6){
            mess =  "ERROR: TestSimpleSimu: 4-Read of updated time for simulation was unsuccessful";
            return 1;
        };
    };

    if(FbsfTerminate(pComp)==Failure || FbsfGetStatus(pComp, &st)==Failure || st!=FbsfTerminated)
    {
        mess =  "ERROR: TestSimpleSimu: 5-Termination step was unsuccessful";
        return 1;
    }
    if(FbsfFreeInstance(&pComp)==Failure || pComp)
    {
        mess =  "ERROR: TestSimpleSimu: 6-FreeInstance step was unsuccessful";
        return 1;
    }
    return 0;
};


int TestSimpleSimuFMU(int ac, char **av, QString& mess)
{
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 0.05;
    const double timeOutCPUs = 1e6;
    const QString simuTimeString = "Simulation.Time";
    double ZEValTime = -1;
    cout << "Starting Test2b - TestSimpleSimuFMU"<<endl;

    void *pComp = FbsfInstantiate("TestFMU_Crash2.xml", ac, av);
    if(!pComp || FbsfGetStatus(pComp, &st)==Failure || st==FbsfUninitialized)
    {
        mess =  "ERROR: TestSimpleSimuFMU: 1-Initialization was not successful";
        return 1;
    };

    if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime) > 1.e-9){
        mess =  "ERROR: TestSimpleSimuFMU: 2-Read of initial time for simulation was unsuccessful";
        return 1;
    };

    for (int yy = 0; yy < 10; yy++){

        if(FbsfDoStep(pComp, timeOutCPUs) == Failure || FbsfGetStatus(pComp, &st)==Failure || st!=FbsfReady){
            mess =  "ERROR: TestSimpleSimuFMU: 3-Timestepping was unsuccessful";
            return 1;
        }

        if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime - (yy+1) * timestep) > 1.e-6){
            mess =  "ERROR: TestSimpleSimuFMU: 4-Read of updated time for simulation was unsuccessful";
            return 1;
        };
    };

    if(FbsfTerminate(pComp)==Failure || FbsfGetStatus(pComp, &st)==Failure || st!=FbsfTerminated)
    {
        mess =  "ERROR: TestSimpleSimuFMU: 5-Termination step was unsuccessful";
        return 1;
    }
    if(FbsfFreeInstance(&pComp)==Failure || pComp)
    {
        mess =  "ERROR: TestSimpleSimuFMU: 6-FreeInstance step was unsuccessful";
        return 1;
    }
    return 0;
};


int TestWallClock(int ac, char **av, QString& mess)
{
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 0.2;
    const double timeOutCPUs = 1e-8;
    const QString simuTimeString = "Simulation.Time";
    double ZEValTime = -1;
    cout << "Starting Test3 - TestWallClock"<<endl;

    void *pComp = FbsfInstantiate("simul.xml", ac, av);
    if(!pComp || FbsfGetStatus(pComp, &st)==Failure || st==FbsfUninitialized)
    {
        mess =  "ERROR: TestWallClock: 1-Initialization was not successful";
        return 1;
    };

    if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime) > 1.e-9){
        mess =  "ERROR: TestWallClock: 2-Read of initial time for simulation was unsuccessful";
        return 1;
    };

    for (int yy = 0; yy < 10; yy++){

        if(FbsfDoStep(pComp, timeOutCPUs) == Failure || FbsfGetStatus(pComp, &st)==Failure){
            mess =  "ERROR: TestWallClock: 3a-Timestep was unsuccessful";
            return 1;
        };

// Specific test is not functional yet, as agreed with L3S
//        if(st!=FbsfTimeOut){
//            mess =  "ERROR: TestWallClock: 3b-Timeout flag was not raised";
//            return 1;
//        };

        if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime - (yy+1) * timestep) > 1.e-6){
            mess =  "ERROR: TestWallClock: 4-Read of updated time for simulation was unsuccessful";
            return 1;
        };
    };

    if(FbsfTerminate(pComp)==Failure || FbsfGetStatus(pComp, &st)==Failure || st!=FbsfTerminated)
    {
        mess =  "ERROR: TestWallClock: 5-Termination step was unsuccessful";
        return 1;
    };
    if(FbsfFreeInstance(&pComp)==Failure || pComp)
    {
        mess =  "ERROR: TestWallClock: 6-FreeInstance step was unsuccessful";
        return 1;
    };
    return 0;
};

int TestSaveRestore(int ac, char **av, QString& mess)
{
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 0.05;
    const double timeOutCPUs = 1e6;
    const QString simuTimeString = "Simulation.Time";
    const QString TimeInt = "OutPutTimeIntegrator";
    const QString TimeValFMU = "Time";
    QStringList ZEkeys = {simuTimeString, TimeInt, TimeValFMU};
    double ZEValTime = -1;
    double ZEFMUTimeInt = -1;
    double ZEFMUTime = -1;
    // Saved data to compare after restore
    double ZEValTimeSave = -1;
    double ZEFMUTimeIntSave = -1;
    double ZEFMUTimeSave = -1;

    cout << "Starting Test4 - TestSaveRestore"<<endl;

    void *pComp = FbsfInstantiate("TestFMU_Crash2.xml", ac, av);
    if(!pComp || FbsfGetStatus(pComp, &st)==Failure || st==FbsfUninitialized)
    {
        mess =  "ERROR: TestSaveRestore: 1-Initialization was not successful";
        return 1;
    };

    QStringList ZEVarList;
    QString element;
    if (FbsfGetDataNames(pComp, &ZEVarList) == Failure){
        mess =  "ERROR: TestSaveRestore: 2-GetDataNames was unsuccessful";
        return 1;
    };

    // Verify that all required keys are indeed present in the exchange zone
    foreach (const QString& ZEkey, ZEkeys) {
        if (ZEVarList.indexOf(ZEkey) == -1)
        {
            mess =  "ERROR: TestSaveRestore: 3-A required key was not found through a dedicated function";
            return 1;
        }
    };

    for (int yy = 0; yy < 60; yy++){

        if(yy == 1){
            FbsfSaveState(pComp);
        };

        if(FbsfDoStep(pComp, timeOutCPUs) == Failure || FbsfGetStatus(pComp, &st)==Failure){
            mess =  "ERROR: TestSaveRestore: 4-Timestepping was unsuccessful";
            return 1;
        };

        // If the simulator crashed, exit the loop for restoration
        if(st==FbsfFailedStep)
        {
            break;
        };

        // Get data from exchange zone
        if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime - (yy+1) * timestep) > 1.e-6
                || FbsfGetRealData(pComp, TimeInt, &ZEFMUTimeInt) == Failure || FbsfGetRealData(pComp, TimeValFMU, &ZEFMUTime) == Failure){
            mess =  "ERROR: TestSaveRestore: 5-Read of updated values from simulation was unsuccessful";
            return 1;
        };

        // Save reference values for output data to compare later on
        if (yy == 1)
        {
            ZEValTimeSave = ZEValTime;

            ZEFMUTimeIntSave = ZEFMUTimeInt;

            ZEFMUTimeSave = ZEFMUTime;
        }

    };
    if (st != FbsfFailedStep){
        mess =  "ERROR: TestSaveRestore: 6-Failed step detection failed";
        return 1;
    };

    // Here no test because of a lack of implementation below
    FbsfRestoreState(pComp);


    if(FbsfDoStep(pComp, timeOutCPUs) == Failure || FbsfGetStatus(pComp, &st)==Failure){
        mess =  "ERROR: TestSaveRestore: 7-Timestepping after failure was unsuccessful";
        return 1;
    };

    if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime - 2 * timestep) > 1.e-6
            || FbsfGetRealData(pComp, TimeInt, &ZEFMUTimeInt) == Failure || FbsfGetRealData(pComp, TimeValFMU, &ZEFMUTime) == Failure){
        mess =  "ERROR: TestSaveRestore: 8-Read of updated values from simulation after failure was unsuccessful";
        return 1;
    };

    if((ZEValTimeSave != ZEValTime) || (ZEFMUTimeIntSave != ZEFMUTimeInt) || (ZEFMUTimeSave != ZEFMUTime)){
        mess =  "ERROR: TestSaveRestore: 9-Updated values from simulation after failure are not compliant";
        return 1;
    };

    if(FbsfTerminate(pComp)==Failure || FbsfGetStatus(pComp, &st)==Failure || st!=FbsfTerminated)
    {
        mess =  "ERROR: TestSaveRestore: 10-Termination step was unsuccessful";
        return 1;
    }
    if(FbsfFreeInstance(&pComp)==Failure || pComp)
    {
        mess =  "ERROR: TestSaveRestore: 11-FreeInstance step was unsuccessful";
        return 1;
    }
    return 0;
};

int TestRestoreWithCorr(int ac, char **av, QString& mess)
{
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 0.05;
    const double timeOutCPUs = 1e6;
    const double tfin = 3.;
    const double tcrash = 2.5;
    int stepcrash = round(tcrash / timestep);
    int stepend = round(tfin / timestep);
    const QString simuTimeString = "Simulation.Time";
    const QString TimeInt = "OutPutTimeIntegrator";
    const QString TimeValFMU = "Time";
    const QString CorrValFMU = "Corr";
    const QString FailOutValFMU = "FailOut";
    QStringList ZEkeys = {simuTimeString, TimeInt, TimeValFMU, CorrValFMU, FailOutValFMU};
    // Current values in exchange zones
    double ZEValTime = -1;
    double ZEFMUTimeInt = -1;
    double ZEFMUTime = -1;
    double ZEFMUCorr = -1;
    double ZEFMUFailOut = -1;

    // Corrected value for handle in order to circumvent the crash
    double Corr2 = 1.1;
    cout << "Starting Test5 - TestRestoreWithCorr"<<endl;

    void *pComp = FbsfInstantiate("TestFMU_Crash2_UnRes.xml", ac, av);
    if(!pComp || FbsfGetStatus(pComp, &st)==Failure || st==FbsfUninitialized)
    {
        mess =  "ERROR: TestRestoreWithCorr: 1-Initialization was not successful";
        return 1;
    };

    QStringList ZEVarList;
    QStringList ZEUnResVarList;

    if (FbsfGetDataNames(pComp, &ZEVarList) == Failure){
        mess =  "ERROR: TestRestoreWithCorr: 2-GetDataNames was unsuccessful";
        return 1;
    };
    if (FbsfGetUnresolvedDataNames(pComp, &ZEUnResVarList) == Failure){
        mess =  "ERROR: TestRestoreWithCorr: 3-GetUnresolvedDataNames was unsuccessful";
        return 1;
    };

    // Verify that all required keys are indeed present in the exchange zone
    foreach (const QString& ZEkey, ZEkeys) {
        if (ZEVarList.indexOf(ZEkey) == -1)
        {
            mess =  "ERROR: TestRestoreWithCorr: 4-A required key was not found through a dedicated function";
            return 1;
        }
    };
    // Verify that the handle we look for is indeed unresolved
    if (ZEUnResVarList.indexOf(CorrValFMU) == -1)
    {
        mess =  "ERROR: TestRestoreWithCorr: 5-The simulation handle was not identified as unresolved";
        return 1;
    }
    // Verify that the same ZE element has the right type and size
    FbsfDataType CorrDType;
    if(FbsfGetDataType(pComp, CorrValFMU, &CorrDType) == Failure || CorrDType != FbsfReal)
    {
        mess =  "ERROR: TestRestoreWithCorr: 6-The simulation unresolved handle does not have the requested type";
        return 1;
    }

    for (int yy = 0; yy < stepend; yy++){
        // Systematic state saving
        FbsfSaveState(pComp);
        if(FbsfDoStep(pComp, timeOutCPUs) == Failure || FbsfGetStatus(pComp, &st)==Failure){
            mess =  "ERROR: TestRestoreWithCorr: 7-Timestepping was unsuccessful";
            return 1;
        };
        // If the simulator crashed, exit the loop for restoration
        if(st==FbsfFailedStep)
        {
            if(abs(yy + 1 - stepcrash) > 1e-6){
                mess =  "ERROR: TestRestoreWithCorr: 8-Crash was not detected on time";
                return 1;
            }
            break;
        };
        // Get data from exchange zone
        if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime - (yy+1) * timestep) > 1.e-6
                || FbsfGetRealData(pComp, TimeInt, &ZEFMUTimeInt) == Failure || FbsfGetRealData(pComp, TimeValFMU, &ZEFMUTime) == Failure
                || FbsfGetRealData(pComp, CorrValFMU, &ZEFMUCorr) == Failure || FbsfGetRealData(pComp, FailOutValFMU, &ZEFMUFailOut)){
            mess =  "ERROR: TestRestoreWithCorr: 9-Read of updated values from simulation was unsuccessful";
            return 1;
        };
    };
    if (st != FbsfFailedStep){
        mess =  "ERROR: TestRestoreWithCorr: 10-Failed step detection failed";
        return 1;
    };

    // Here no test because of a lack of implementation below
    FbsfRestoreState(pComp);

    // Set value for Corr to an acceptable level
    if (FbsfSetRealData(pComp, CorrValFMU, Corr2) == Failure){
        mess =  "ERROR: TestRestoreWithCorr: 11-Modifying an entry was unsuccessful";
        return 1;
    }

    // Now loop over remaining timesteps
    for (int yy = stepcrash; yy < stepend + 1; yy++)
    {
        if(FbsfDoStep(pComp, timeOutCPUs) == Failure || FbsfGetStatus(pComp, &st)==Failure){
            mess =  "ERROR: TestRestoreWithCorr: 12-Timestepping after failure was unsuccessful";
            return 1;
        };
        if (st==FbsfFailedStep)
        {
            mess =  "ERROR: TestRestoreWithCorr: 13-Timestepping repeatedly failed after input correction";
            return 1;
        }
    }
    // Check that the final time is the one expected
    if(FbsfGetRealData(pComp, simuTimeString, &ZEValTime) == Failure || abs(ZEValTime - stepend * timestep) > 1.e-4
            || FbsfGetRealData(pComp, TimeValFMU, &ZEFMUTime) == Failure ||  abs(ZEFMUTime - stepend * timestep) > 1.e-4)
    {
        mess =  "ERROR: TestRestoreWithCorr: 14-Final time was not reached in the application";
        return 1;
    }

    if(FbsfTerminate(pComp)==Failure || FbsfGetStatus(pComp, &st)==Failure || st!=FbsfTerminated)
    {
        mess =  "ERROR: TestRestoreWithCorr: 15-Termination step was unsuccessful";
        return 1;
    }
    if(FbsfFreeInstance(&pComp)==Failure || pComp)
    {
        mess =  "ERROR: TestRestoreWithCorr: 16-FreeInstance step was unsuccessful";
        return 1;
    }
    return 0;
};


int main(int ac, char **av) {
    void *comp = nullptr;
    std::string cmd;
    QString mess;
    // overload inputs to ensure value
    //    ac = 1;
    //    av[0] = (char *)("no-gui");
    cout << "command:";
    while (getline(cin, cmd)){
        if (cmd== "") {
            int error = TestLoadSuccess(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            error = TestLoadFailure(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            error = TestSimpleSimu(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            error = TestSimpleSimuFMU(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            error = TestWallClock(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            error = TestSaveRestore(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            error = TestRestoreWithCorr(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: All tests passed"));
            return 0;
        }
        if (cmd == "t1") {
            int error = TestLoadSuccess(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: TestLoadSuccess"));
            return 0;
        }
        if (cmd == "t1b") {
            int error = TestLoadFailure(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: TestLoadFailure"));
            return 0;
        }
        if (cmd == "t2") {
            int error = TestSimpleSimu(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: TestSimpleSimu"));
            return 0;
        }
        if (cmd == "t2b") {
            int error = TestSimpleSimuFMU(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: TestSimpleSimuFMU"));
            return 0;
        }
        if (cmd == "t3") {
            int error = TestWallClock(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: TestWallClock"));
            return 0;
        }
        if (cmd == "t4") {
            int error = TestSaveRestore(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: TestSaveRestore"));
            return 0;
        }
        if (cmd == "t5") {
            int error = TestRestoreWithCorr(ac, av, mess);
            if (error)
            {
                qInfo(qPrintable(mess));
                return 1;
            }
            qInfo(qPrintable("SUCCESS: TestRestoreWithCorr"));
            return 0;
        }

    }
    cout << "ended"<<endl;
};
