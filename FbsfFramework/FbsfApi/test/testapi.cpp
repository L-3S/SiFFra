#include "testapi.h"
#include <QApplication>
#include <string>
int main(int ac, char **av) {
    cout << "instanciate"<<endl;
    FbsfApi api;
    void *comp = api.instanciate(ac, av);
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
        } else if (cmd == "st") {
            fmi2String str = nullptr;
//            fmi2String *str = (fmi2String*)calloc(1, sizeof(fmi2String));
            api.fmi2GetStringStatus(comp, fmi2PendingStatus, &str);
            if (str) {
                std::cout << "status :" << str<<std::endl;
            }
        } else if (cmd == "f") {
            api.fmi2FreeInstance(comp);
        } else if (cmd == "quit") {
            return 0;
        }
        cout << "command:";
    }
    cout << "ended"<<endl;
}
