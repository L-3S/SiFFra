#include "testapi.h"
#include <string>
QString const defaultSimuFile = "TestFMU_Crash2.xml"; //"simul.xml"
int Test1a(int ac, char **av) {
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    do{
        cout << "Starting Test1a"<<endl;
        void *pComp = FbsfInstantiate("simul.xml", ac, av);
        failure += !pComp ? 1 : 0;
        if(failure) break;

        su = FbsfGetStatus(pComp, &st);

        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;
        // Terminate + Unload
        su = FbsfTerminate(pComp) ;
        failure += (su==StepFailure) ? 4 : 0;
        if(failure) break;
        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 8 : 0;
        if(failure) break;

        std::cout << pComp << std::endl;

        su = FbsfFreeInstance(&pComp) ;
        std::cout << pComp << std::endl;

        failure += (su==StepFailure || pComp) ? 16 : 0;
        if(failure) break;
    } while(0);
    return failure;
}

int Test1b(int ac, char **av) {
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    do{

        cout << "Starting Test1b"<<endl;
        void *pComp = FbsfInstantiate("simulLALA.xml", ac, av);
        qDebug() << "Starting Test1b2";

        failure += !pComp ? 1 : 0;
        if(failure) break;
        qDebug() << "Starting Test1b3";

        su = FbsfGetStatus(pComp, &st);
        qDebug() << "Starting Test1b4";
        failure += (su==StepFailure || st!=FbsfUninitialized) ? 2 : 0;
        qDebug() << "Starting Test1b5";
        if(failure) break;
        qDebug() << "Starting Test1b6";
        FbsfTerminate(pComp);

        qDebug() << "Starting Test1b7";
        su = FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 4 : 0;
        if(failure) break;
    } while(0);

    return failure;
}

// Test2: specs E1-2-1, E1-3, E1-7-1
// Simulation on multiple timesteps
int Test2(int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 10.;
    const double timeOutCPUs = 10;

    const string simuTimeString = "Simulation.Time";
    do{

        cout << "Starting Test2"<<endl;

        void *pComp = FbsfInstantiate("simul.xml", ac, av);
        failure += !pComp ? 1 : 0;
        if(failure) break;


        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;

        // Get id for time variable
//        fbsfValueReference timeRef = fbsfGetRef(pComp, simuTimeString);
//        failure += (timeRef == -1) ? 4: 0;
//        if(failure) break;

        double pZEValTime = -1;
        qDebug() << "Antoine prerecup";
//        this_thread::sleep_for(chrono::milliseconds(2000) );
        FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
        failure += pZEValTime == -1 ? 8 : 0;
        if(failure) break;

        // One step increment
        su = FbsfDoStep(pComp, timeOutCPUs);
        failure += su == StepFailure ? 16 : 0;
        if(failure) break;

        su = FbsfGetStatus(pComp, &st);
        FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
        std::cout << "Antoine Res " << su <<" " << st <<" " << abs(pZEValTime - timestep) << std::endl;
        failure += (su==StepFailure || st!=FbsfReady || abs(pZEValTime - timestep) > 1.e-9) ? 32 : 0;
        if(failure) break;

        // Ten step increment
        for (int y = 0; y < 10; y++) su = FbsfDoStep(pComp, timeOutCPUs);
        failure += su == StepFailure ? 64 : 0;
        if(failure) break;

        su = FbsfGetStatus(pComp, &st);
        FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
        failure += (su==StepFailure || st!=FbsfReady || abs(pZEValTime - 11. * timestep) > 1.e-9) ? 128 : 0;
        if(failure) break;

        // Terminate + Unload
        su = FbsfTerminate(pComp) ;
        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 256 : 0;
        if(failure) break;

        su = FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 512 : 0;
        if(failure) break;

    } while(0);
    return failure;
}

int Test2b(int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timestep = 10.;
    const double timeOutCPUs = 10;

    const string simuTimeString = "resetTet";
    do{

        cout << "Starting Test2"<<endl;

        void *pComp = FbsfInstantiate("simul.xml", ac, av);
        failure += !pComp ? 1 : 0;
        if(failure) break;


        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;

        // Get id for time variable
//        fbsfValueReference timeRef = fbsfGetRef(pComp, simuTimeString);
//        failure += (timeRef == -1) ? 4: 0;
//        if(failure) break;

        int pZEValTime = -1;
        qDebug() << "Antoine prerecup";
//        this_thread::sleep_for(chrono::milliseconds(2000) );
        FbsfGetIntegerData(pComp, simuTimeString.c_str(), &pZEValTime);
        failure += pZEValTime == -1 ? 8 : 0;
        if(failure) break;

        // One step increment
        su = FbsfDoStep(pComp, timeOutCPUs);
        failure += su == StepFailure ? 16 : 0;
        if(failure) break;
        FbsfSaveState(pComp);

        // Ten step increment
        for (int y = 0; y < 10; y++) {
            su = FbsfDoStep(pComp, timeOutCPUs);
            FbsfRestoreState(pComp);
            FbsfGetIntegerData(pComp, simuTimeString.c_str(), &pZEValTime);
            qDebug() << "Antoine data " << pZEValTime;
        }
        failure += su == StepFailure ? 64 : 0;
        if(failure) break;

        su = FbsfGetStatus(pComp, &st);
        FbsfGetIntegerData(pComp, simuTimeString.c_str(), &pZEValTime);
        failure += (su==StepFailure || st!=FbsfReady || pZEValTime > 2) ? 128 : 0;
        if(failure) break;

        // Terminate + Unload
        su = FbsfTerminate(pComp) ;
        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 256 : 0;
        if(failure) break;

        su = FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 512 : 0;
        if(failure) break;

    } while(0);
    return failure;
}


int Test3(int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timeOutCPUs = 0.000001;
    do{
        cout << "Starting Test3"<<endl;
        void *pComp = FbsfInstantiate("simul.xml", ac, av);

        su = FbsfGetStatus(pComp, &st);
        if (su != StepFailure && st == FbsfReady)
        {
            su = FbsfDoStep(pComp, timeOutCPUs);
            failure += (su == StepFailure) ? 2 : 0;
            if(failure) break;

            su = FbsfGetStatus(pComp, &st);
            failure += (su == StepFailure || st != FbsfTimeOut) ? 4 : 0;
            if(failure) break;

            // Terminate + Unload
            su = FbsfTerminate(pComp) ;
            su = FbsfGetStatus(pComp, &st);
            failure += (su==StepFailure || st!=FbsfTerminated) ? 8 : 0;
            if(failure) break;

            su = FbsfFreeInstance(&pComp) ;
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

int Test4(int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timeOutCPUs = 10;
    const double expectedVal = 15; // arbitrary here

    const string simuZEValString = "Pump.Speed";

    do{
        cout << "Starting Test4"<<endl;

        void *pComp = FbsfInstantiate("simul.xml", ac, av);

        // One step increment
        failure += !pComp ? 1 : 0;
        if(failure) break;

        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;


        double pZEValRef = -1;
        FbsfGetRealData(pComp, simuZEValString.c_str(), &pZEValRef);
        failure += pZEValRef == -1 ? 8 : 0;
        if(failure) break;

        // One step increment
        su = FbsfDoStep(pComp, timeOutCPUs);
        failure += su == StepFailure ? 16 : 0;
        if(failure) break;
        FbsfGetRealData(pComp, simuZEValString.c_str(), &pZEValRef);

        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady || abs(pZEValRef - expectedVal) > 1.e-5) ? 32 : 0;
        if(failure) break;

        // Terminate + Unload
        su = FbsfTerminate(pComp) ;
        su = FbsfGetStatus(pComp,&st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 64 : 0;
        if(failure) break;

        su = FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 128 : 0;
        if(failure) break;

    } while(0);
    return failure;
}

int fmutest1(int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timeOutCPUs = 0.000001;
    double pZEValTime = -1;
    double pZEFMUTimeInt = -1;
    QString TimeInt = QString::fromStdString("OutPutTimeIntegrator");
    const string simuTimeString = "Simulation.Time";
    do{
        cout << "Starting Test3, seeking crash"<<endl;
        void *pComp = FbsfInstantiate("TestFMU_Crash2.xml", ac, av);

        su = FbsfGetStatus(pComp, &st);
        if (su != StepFailure && st == FbsfReady)
        {
            QStringList ZEVarList;
            QString element;
            su = FbsfGetDataNames(pComp, &ZEVarList);
            for (int i = 0; i < ZEVarList.size(); ++i){
                      element = ZEVarList.at(i);
                      std::cout << "ZE includes key" << element.toStdString().c_str() <<std::endl;}
            assert(su != StepFailure);
            for (int y = 0; y < 60; y++) {
                if (y == 1){
                FbsfSaveState(pComp);
                }
                    // save at 2s
                su = FbsfDoStep(pComp, timeOutCPUs);
                failure += (su == StepFailure) ? 2 : 0;
                if(failure) break;
                FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
                FbsfGetRealData(pComp, TimeInt, &pZEFMUTimeInt);
                su = FbsfGetStatus(pComp, &st);
                failure += (su == StepFailure || st == FbsfFailedStep) ? 4 : 0;
                if(failure) break;
            }
            if(failure){
                FbsfRestoreState(pComp);
                // See where we are
                FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
                FbsfGetRealData(pComp, TimeInt, &pZEFMUTimeInt);

                // Do one more step for verifying everything is ok
                su = FbsfDoStep(pComp, timeOutCPUs);
                failure += (su == StepFailure) ? 2 : 0;
                FbsfGetRealData(pComp, simuTimeString.c_str(), &pZEValTime);
                FbsfGetRealData(pComp, TimeInt, &pZEFMUTimeInt);

                // Here make a test for verifying that the restore+subsequent dostep were successful
                su = FbsfGetStatus(pComp, &st);
                failure += (su == StepFailure || st == FbsfFailedStep) ? 4 : 0;
                if(failure) break;
            }
            // Terminate + Unload
            su = FbsfTerminate(pComp) ;
            su = FbsfGetStatus(pComp, &st);
            failure += (su==StepFailure || st!=FbsfTerminated) ? 8 : 0;
            if(failure) break;

            su = FbsfFreeInstance(&pComp) ;
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

int fmutest2(int ac, char **av){
    int failure = 0;
    FbsfSuccess su = StepSuccess;
    FbsfStatus st = FbsfUninitialized;
    const double timeOutCPUs = 10;
    const double expectedVal = 15; // arbitrary here

    const string simuZEValString = "Time";

    do{
        cout << "Starting Test4"<<endl;

        void *pComp = FbsfInstantiate("TestFMU_Crash2.xml", ac, av);

        // One step increment
        failure += !pComp ? 1 : 0;
        if(failure) break;

        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady) ? 2 : 0;
        if(failure) break;


        double pZEValRef = -1;
        FbsfGetRealData(pComp, simuZEValString.c_str(), &pZEValRef);
        failure += pZEValRef == -1 ? 8 : 0;
        if(failure) break;

        // Ten step increment
        for (int y = 0; y < 10; y++) {
            su = FbsfDoStep(pComp, timeOutCPUs);
            failure += su == StepFailure ? 16 : 0;
            if(failure) break;
            FbsfGetRealData(pComp, simuZEValString.c_str(), &pZEValRef);
        }
        su = FbsfGetStatus(pComp, &st);
        failure += (su==StepFailure || st!=FbsfReady || abs(pZEValRef - expectedVal) > 1.e-5) ? 32 : 0;
        if(failure) break;

        // Terminate + Unload
        su = FbsfTerminate(pComp) ;
        su = FbsfGetStatus(pComp,&st);
        failure += (su==StepFailure || st!=FbsfTerminated) ? 64 : 0;
        if(failure) break;

        su = FbsfFreeInstance(&pComp) ;
        failure += (su==StepFailure || pComp) ? 128 : 0;
        if(failure) break;

    } while(0);
    return failure;
}



int main(int ac, char **av) {
    cout << "instanciate"<<endl;
//    FbsfApi api;

    void *comp = nullptr;
    std::string cmd;
    cout << "command:";
    while (getline(cin, cmd)){
        if (cmd == "fmu2") {
            int err = fmutest2(ac, av);
            if (err) {
                std::cout << "fmu2 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test fmu2 sucess" << std::endl;
            return 0;
        }
        if (cmd == "fmu1") {
            int err = fmutest1(ac, av);
            if (err) {
                std::cout << "fmu1 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test fmu1 sucess" << std::endl;
            return 0;
        }
        if (cmd == "t1") {
            int err = Test1a(ac, av);
            if (err) {
                std::cout << "Test1a err" << err <<std::endl;
                return err;
            }
            std::cout << "Test 1a sucess" << std::endl;
            return 0;
        }
        if (cmd == "t1b") {
            int err = Test1b(ac, av);

            if (err) {
                std::cout << "Test1b err" << err <<std::endl;
                return err;
            }
            std::cout << "Test 1b sucess" << std::endl;
            return 0;
        }
        if (cmd == "t2") {
            int err = Test2(ac, av);
            if (err) {
                std::cout << "Test2 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test2 sucess" << std::endl;
            return 0;
        }
        if (cmd == "t2b") {
            int err = Test2b(ac, av);
            if (err) {
                std::cout << "Test2b err" << err <<std::endl;
                return err;
            }
            std::cout << "Test2b sucess" << std::endl;
            return 0;
        }
        if (cmd == "t3") {
            int err = Test3(ac, av);
            if (err) {
                std::cout << "Test3 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test3 sucess" << std::endl;
            return 0;
        }
        if (cmd == "t4") {
            int err = Test4(ac, av);
            if (err) {
                std::cout << "Test4 err" << err <<std::endl;
                return err;
            }
            std::cout << "Test4 sucess" << std::endl;
            return 0;
        }
        if (cmd == "data") {

            bool scalar = true;
            bool integ = true;
            cout << "vector or scalar? default: scalar:";
            getline(cin, cmd);
            if (cmd == "vector") {
                scalar = false;
            }
            cout << "int or float? default: int:";
            getline(cin, cmd);
            if (cmd == "float") {
                integ = false;
            }
            cout << "What data? default: Executive:CpuStepTime :";
            getline(cin, cmd);
            if (cmd == "") {
                cmd = "Executive:CpuStepTime";
            }
            if (scalar && integ) {
                int value = 0;
                std::cout << FbsfGetIntegerData(comp, cmd.c_str(), &value) << " " << value << std::endl;
            }
            if (scalar && !integ) {
                double value = 0;
                std::cout << FbsfGetRealData(comp, cmd.c_str(), &value) << " " << value << std::endl;
            }
            if (!scalar && integ) {
                int size = 0;
                FbsfGetDataSize(comp, cmd.c_str(), &size);
                QVector<int> value = QVector<int>(size);
                qDebug() << FbsfGetVectorIntegerData(comp, cmd.c_str(), &value) << " " << value;
            }
            if (!scalar && !integ) {
                int size = 0;
                FbsfGetDataSize(comp, cmd.c_str(), &size);
                QVector<double> value = QVector<double>(size);
                qDebug() << FbsfGetVectorRealData(comp, cmd.c_str(), &value) << " " << value;
            }

        }
        if (cmd == "load" || cmd == "l") {
            cout << "What file? default:"+ defaultSimuFile.toStdString() + ":";
            getline(cin, cmd);
            if (cmd == "") {
                cmd =defaultSimuFile.toStdString();
            }
            cout << "loading " << cmd << endl;
            comp = FbsfInstantiate(QString::fromStdString(cmd), ac, av);
        } else if (cmd == "s") {
            cout << "set a timout ? default:1000 :";
            getline(cin, cmd);
            if (cmd == "") {
                cmd = "1000";
            }
            std::cout << "Start step" << std::endl;
            FbsfDoStep(comp, QString::fromStdString(cmd).toInt());
            std::cout << "Step over" << std::endl;
        } else if (cmd == "restore") {
            FbsfRestoreState(comp);
        } else if (cmd == "save") {
            FbsfSaveState(comp);
        } else if (cmd == "term") {
            FbsfTerminate(comp);
        } else if (cmd == "cancel" || cmd == "c") {
            FbsfCancelStep(comp);
        } else if (cmd == "st") {
            FbsfStatus s;
//            FbsfString *str = (FbsfString*)calloc(1, sizeof(FbsfString));
            FbsfGetStatus(comp, &s);
            std::cout << "status :" << s <<std::endl;
        } else if (cmd == "f") {
            FbsfFreeInstance(&comp);
        } else if (cmd == "quit") {
            return 0;
        }
        cout << "command:";
    }
    cout << "ended"<<endl;
}
