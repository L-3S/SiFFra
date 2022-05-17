#include "testapi.h"
#include <string>
int Test1a(FbsfApi &api, int ac, char **av) {
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    do{
        cout << "Starting Test1a"<<endl;
        void *pComp = api.FbsfInstantiate("simul.xml");
        failure += !pComp ? 1 : 0;
        if(failure) break;

        su = api.FbsfGetStatus(pComp, &st);

        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;
        // Terminate + Unload
        su = api.FbsfTerminate(pComp) ;
        failure += (su==StepFailure) ? 4 : 0;
        if(failure) break;
        su = api.FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 8 : 0;
        if(failure) break;

        std::cout << pComp << std::endl;

        su = api.FbsfFreeInstance(&pComp) ;
        std::cout << pComp << std::endl;

        failure += (su==StepFailure || pComp) ? 16 : 0;
        if(failure) break;
    } while(0);
    return failure;
}

int Test1b(FbsfApi &api, int ac, char **av) {
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    do{

        cout << "Starting Test1b"<<endl;
        void *pComp = api.FbsfInstantiate("simulLALA.xml");
        cout << "Starting Test1b2"<<endl;

        failure += !pComp ? 1 : 0;
        if(failure) break;
        cout << "Starting Test1b3"<<endl;

        su = api.FbsfGetStatus(pComp, &st);
        cout << "Starting Test1b4"<<endl;
        failure += (su==StepFailure || st!=FbsfUninitialized) ? 2 : 0;
        cout << "Starting Test1b5"<<endl;
        if(failure) break;
        cout << "Starting Test1b6"<<endl;
        api.FbsfTerminate(pComp);
        su = api.FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 4 : 0;
        if(failure) break;
    } while(0);

    return failure;
}

// Test2: specs E1-2-1, E1-3, E1-7-1
// Simulation on multiple timesteps
int Test2(FbsfApi &api, int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 10.;
    const double timeOutCPUs = 10;

    const string simuTimeString = "Simulation.Time";
    do{

        cout << "Starting Test2"<<endl;

        void *pComp = api.FbsfInstantiate("simul.xml");
        failure += !pComp ? 1 : 0;
        if(failure) break;


        su = api.FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;

        // Get id for time variable
//        fbsfValueReference timeRef = fbsfGetRef(pComp, simuTimeString);
//        failure += (timeRef == -1) ? 4: 0;
//        if(failure) break;

        double pZEValTime = -1;
        api.FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
        failure += pZEValTime == -1 ? 8 : 0;
        if(failure) break;

        // One step increment
        su = api.FbsfDoStep(pComp, timeOutCPUs);
        failure += su == StepFailure ? 16 : 0;
        if(failure) break;

        su = api.FbsfGetStatus(pComp, &st);
        api.FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
        std::cout << "Antoine Res " << su <<" " << st <<" " << abs(pZEValTime - timestep) << std::endl;
        failure += (su==StepFailure || st!=FbsfReady || abs(pZEValTime - timestep) > 1.e-9) ? 32 : 0;
        if(failure) break;

        // Ten step increment
        for (int y = 0; y < 10; y++) {
            su = api.FbsfDoStep(pComp, timeOutCPUs);
        }
        failure += su == StepFailure ? 64 : 0;
        if(failure) break;

        su = api.FbsfGetStatus(pComp, &st);
        api.FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
        failure += (su==StepFailure || st!=FbsfReady || abs(pZEValTime - 11. * timestep) > 1.e-9) ? 128 : 0;
        if(failure) break;

        // Terminate + Unload
        su = api.FbsfTerminate(pComp) ;
        su = api.FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 256 : 0;
        if(failure) break;

        su = api.FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 512 : 0;
        if(failure) break;

    } while(0);
    return failure;
}

int Test3(FbsfApi &api, int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timeOutCPUs = 0.000001;
    do{
        cout << "Starting Test3"<<endl;
        void *pComp = api.FbsfInstantiate("simul.xml");

        su = api.FbsfGetStatus(pComp, &st);
        if (su != StepFailure && st == FbsfReady)
        {
            su = api.FbsfDoStep(pComp, timeOutCPUs);
            failure += (su == StepFailure) ? 2 : 0;
            if(failure) break;

            su = api.FbsfGetStatus(pComp, &st);
            failure += (su == StepFailure || st != FbsfTimeOut) ? 4 : 0;
            if(failure) break;

            // Terminate + Unload
            su = api.FbsfTerminate(pComp) ;
            su = api.FbsfGetStatus(pComp, &st);
            failure += (su==StepFailure || st!=FbsfTerminated) ? 8 : 0;
            if(failure) break;

            su = api.FbsfFreeInstance(&pComp) ;
            failure += (su==StepFailure || pComp) ? 16 : 0;
            if(failure) break;
        }
        else
        {
            failure = 1;
        }
    }while(0);
    return failure;
}

int Test4(FbsfApi &api, int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timeOutCPUs = 10;
    const double expectedVal = 15; // arbitrary here

    const string simuZEValString = "Pump.Speed";

    do{
        cout << "Starting Test4"<<endl;

        void *pComp = api.FbsfInstantiate("simul.xml");

        // One step increment
        failure += !pComp ? 1 : 0;
        if(failure) break;

        su = api.FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;


        double pZEValRef = -1;
        api.FbsfGetRealData(pComp, simuZEValString.c_str(), &pZEValRef);
        failure += pZEValRef == -1 ? 8 : 0;
        if(failure) break;

        // One step increment
        su = api.FbsfDoStep(pComp, timeOutCPUs);
        failure += su == StepFailure ? 16 : 0;
        if(failure) break;
        api.FbsfGetRealData(pComp, simuZEValString.c_str(), &pZEValRef);

        su = api.FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady || abs(pZEValRef - expectedVal) > 1.e-5) ? 32 : 0;
        if(failure) break;

        // Terminate + Unload
        su = api.FbsfTerminate(pComp) ;
        su = api.FbsfGetStatus(pComp,&st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 64 : 0;
        if(failure) break;

        su = api.FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 128 : 0;
        if(failure) break;

    } while(0);
    return failure;
}


int main(int ac, char **av) {
    cout << "instanciate"<<endl;
    FbsfApi api;

    void *comp = nullptr;
    std::string cmd;
    cout << "command:";
    while (getline(cin, cmd)){
        if (cmd == "t1") {
            int err = Test1a(api, ac, av);
            if (err) {
                std::cout << "Test1a err" << err <<std::endl;
                return err;
            }
            std::cout << "Test 1a sucess" << std::endl;
            return 0;
        }
        if (cmd == "t1b") {
            int err = Test1b(api, ac, av);

            if (err) {
                std::cout << "Test1b err" << err <<std::endl;
                return err;
            }
            std::cout << "Test 1b sucess" << std::endl;
            return 0;
        }
        if (cmd == "t2") {
            int err = Test2(api, ac, av);
            if (err) {
                std::cout << "Test2 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test2 sucess" << std::endl;
            return 0;
        }
        if (cmd == "t3") {
            int err = Test3(api, ac, av);
            if (err) {
                std::cout << "Test3 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test3 sucess" << std::endl;
            return 0;
        }
        if (cmd == "t4") {
            int err = Test4(api, ac, av);
            if (err) {
                std::cout << "Test4 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test4 sucess" << std::endl;
            return 0;
        }
        if (cmd == "data") {
            double value = 0;
            cout << "What data? default: Executive:CpuStepTime :";
            getline(cin, cmd);
            if (cmd == "") {
                cmd = "Executive:CpuStepTime";
            }
            std::cout << api.FbsfGetRealData(comp, cmd.c_str(), &value) << " " << value << std::endl;

        }
        if (cmd == "load" || cmd == "l") {
            cout << "What file? default:simul.xml :";
            getline(cin, cmd);
            if (cmd == "") {
                cmd = "simul.xml";
            }
            cout << "loading " << cmd << endl;
            comp = api.FbsfInstantiate(QString::fromStdString(cmd));
        } else if (cmd == "s") {
            cout << "set a timout ? default:1000 :";
            getline(cin, cmd);
            if (cmd == "") {
                cmd = "1000";
            }
            api.FbsfSetString(comp, QString::fromStdString(cmd));
            std::cout << "Start step" << std::endl;
            api.FbsfDoStep(comp, QString::fromStdString(cmd).toInt());
            std::cout << "Step over" << std::endl;
        } else if (cmd == "term") {
            api.FbsfTerminate(comp);
        } else if (cmd == "cancel" || cmd == "c") {
            api.FbsfCancelStep(comp);
        } else if (cmd == "sts") {
            FbsfString str = nullptr;
//            FbsfString *str = (FbsfString*)calloc(1, sizeof(FbsfString));
            api.FbsfGetStringStatus(comp, FbsfPendingStatus, &str);
            if (str) {
                std::cout << "string status :" << str<<std::endl;
            }
        } else if (cmd == "stb") {
            FbsfBoolean b = FbsfFalse;
//            FbsfString *str = (FbsfString*)calloc(1, sizeof(FbsfString));
            api.FbsfGetBooleanStatus(comp, FbsfIsTerminated, &b);
            std::cout << "boolean status :" << b <<std::endl;
        } else if (cmd == "st") {
            FbsfStatus s;
//            FbsfString *str = (FbsfString*)calloc(1, sizeof(FbsfString));
            api.FbsfGetStatus(comp, &s);
            std::cout << "status :" << s <<std::endl;
        } else if (cmd == "stl") {
            FbsfReal r = 0;
//            FbsfString *str = (FbsfString*)calloc(1, sizeof(FbsfString));
            api.FbsfGetRealStatus(comp, FbsfLastSuccessfulTime, &r);
            std::cout << "last successful step:" << r <<std::endl;
        } else if (cmd == "f") {
            api.FbsfFreeInstance(&comp);
        } else if (cmd == "quit") {
            return 0;
        }
        cout << "command:";
    }
    cout << "ended"<<endl;
}
