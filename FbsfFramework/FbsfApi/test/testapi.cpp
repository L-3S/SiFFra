#include "testapi.h"
#include <iostream>
using namespace std;
TestAPI::TestAPI()
{

}

int main(int ac, char **av) {
    FbsfApi api;
    cout << "instanciate"<<endl;
    void *comp = api.instanciate(ac, av);
    cout << "set str"<<endl;
    api.fmi2SetString(comp, "./simul.xml");
    cout << "Init"<<endl;
    api.fmi2EnterInitialisationMode(comp);
    cout << "run"<<endl;
    api.fmi2ExitInitialisationMode(comp);
    cout << "als"<<endl;
    _sleep(10000);
    while (1) {
        api.fmi2DoStep(comp);
        _sleep(1000);
    }
    api.fmi2Terminate(comp);
    cout << "ended"<<endl;
}
