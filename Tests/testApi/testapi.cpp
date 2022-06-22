#include "testapi.h"
#include <string>
int main(int ac, char **av) {
    cout << "instanciate"<<endl;
    FbsfApi api;
    void *comp = api.FbsfInstanciate(ac, av);
    std::string cmd;
    cout << "command:";
    while (getline(cin, cmd)){
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
            std::cout << "Start step" << std::endl;
            api.FbsfDoStep(comp);
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
            api.FbsfGetBooleanStatus(comp, FbsfTerminated, &b);
            std::cout << "boolean status :" << b <<std::endl;
        } else if (cmd == "st") {
            FbsfStatus s = FbsfOK;
//            FbsfString *str = (FbsfString*)calloc(1, sizeof(FbsfString));
            api.FbsfGetStatus(comp, FbsfDoStepStatus, &s);
            std::cout << "status :" << s <<std::endl;
        } else if (cmd == "stl") {
            FbsfReal r = 0;
//            FbsfString *str = (FbsfString*)calloc(1, sizeof(FbsfString));
            api.FbsfGetRealStatus(comp, FbsfLastSuccessfulTime, &r);
            std::cout << "last successful step:" << r <<std::endl;
        } else if (cmd == "f") {
            api.FbsfFreeInstance(comp);
        } else if (cmd == "quit") {
            return 0;
        }
        cout << "command:";
    }
    cout << "ended"<<endl;
}
