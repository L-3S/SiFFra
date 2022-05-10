#include "testapi.h"
#include <string>
int Test1a(FbsfApi &api, int ac, char **av) {
    int failure = 0;
        FbsfSuccess su = StepSuccess;
        FbsfStatus st = FbsfUninitialized;
        do{
            cout << "Starting Test1a"<<endl;
            void *pComp = api.FbsfInstantiate(ac, av);
            failure += !pComp ? 1 : 0;
            if(failure) break;

            api.FbsfSetString(pComp, "simul.xml");
            api.FbsfEnterInitialisationMode(pComp);
            std::this_thread::sleep_for(2000ms);
            api.FbsfExitInitialisationMode(pComp);
            su = api.FbsfGetStatus(pComp, FbsfApiStatus, &st);

            failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
            if(failure) break;
            // Terminate + Unload
            su = api.FbsfTerminate(pComp) ;
            failure += (su==StepFailure) ? 4 : 0;
            if(failure) break;
            su = api.FbsfGetStatus(pComp, FbsfApiStatus, &st);
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


int main(int ac, char **av) {
    cout << "instanciate"<<endl;
    FbsfApi api;

    void *comp = api.FbsfInstantiate(ac, av);
    std::string cmd;
    cout << "command:";
    while (getline(cin, cmd)){
        if (cmd == "t1") {
            int err = Test1a(api, ac, av);
            std::cout << "Antoine ou of test 1";
            if (err) {
                std::cout << "Test1 err" << err <<std::endl;
                return err;
            }
            return 0;
        }
        if (cmd == "data") {
            double value = 0;
            cout << "What data? default: Executive:CpuStepTime :";
            getline(cin, cmd);
            if (cmd == "") {
                cmd = "Executive:CpuStepTime";
            }
            std::cout << api.FbsfGetRealData(cmd.c_str(), &value) << " " << value << std::endl;

        }
        if (cmd == "load" || cmd == "l") {
            cout << "What file? default:simul.xml :";
            getline(cin, cmd);
            if (cmd == "") {
                cmd = "simul.xml";
            }
            cout << "loading " << cmd << endl;
            api.FbsfSetString(comp, QString::fromStdString(cmd));
        } else if (cmd == "init" || cmd == "i") {
            cout << "Init"<<endl;
            api.FbsfEnterInitialisationMode(comp);
        } else if (cmd == "run") {
            api.FbsfExitInitialisationMode(comp);
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
            api.FbsfGetStatus(comp, FbsfDoStepStatus, &s);
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
