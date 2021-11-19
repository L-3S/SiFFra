#include "testapi.h"
#include <string>
int main(int ac, char **av) {
    cout << "instanciate"<<endl;
    FbsfApi api;
    void *comp = api.fmi2Instanciate(ac, av);
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
            api.fmi2SetString(comp, QString::fromStdString(cmd));
        } else if (cmd == "init" || cmd == "i") {
            cout << "Init"<<endl;
            api.fmi2EnterInitialisationMode(comp);
        } else if (cmd == "run") {
            api.fmi2ExitInitialisationMode(comp);
        } else if (cmd == "s") {
            api.fmi2DoStep(comp);
        } else if (cmd == "term") {
            api.fmi2Terminate(comp);
        } else if (cmd == "cancel" || cmd == "c") {
            api.fmi2CancelStep(comp);
        } else if (cmd == "sts") {
            fmi2String str = nullptr;
//            fmi2String *str = (fmi2String*)calloc(1, sizeof(fmi2String));
            api.fmi2GetStringStatus(comp, fmi2PendingStatus, &str);
            if (str) {
                std::cout << "string status :" << str<<std::endl;
            }
        } else if (cmd == "stb") {
            fmi2Boolean b = fmi2False;
//            fmi2String *str = (fmi2String*)calloc(1, sizeof(fmi2String));
            api.fmi2GetBooleanStatus(comp, fmi2Terminated, &b);
            std::cout << "boolean status :" << b <<std::endl;
        } else if (cmd == "st") {
            fmi2Status s = fmi2OK;
//            fmi2String *str = (fmi2String*)calloc(1, sizeof(fmi2String));
            api.fmi2GetStatus(comp, fmi2DoStepStatus, &s);
            std::cout << "status :" << s <<std::endl;
        } else if (cmd == "stl") {
            fmi2Real r = 0;
//            fmi2String *str = (fmi2String*)calloc(1, sizeof(fmi2String));
            api.fmi2GetRealStatus(comp, fmi2LastSuccessfulTime, &r);
            std::cout << "last successful step:" << r <<std::endl;
        } else if (cmd == "f") {
            api.fmi2FreeInstance(comp);
        } else if (cmd == "quit") {
            return 0;
        }
        cout << "command:";
    }
    cout << "ended"<<endl;
}
