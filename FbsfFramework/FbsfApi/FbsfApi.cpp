#include "FbsfApi.h"
#include "FbsfNetServer.h"
#include "FbsfExecutive.h"
#include <QtCore/QList>
#include <iostream>
#include "FbsfApplication.h"
#include "FbsfConfiguration.h"

FbsfControllerComponent::FbsfControllerComponent(int aac, char **aav): ac(aac), av(aav), app(nullptr), run(false), threadRunning(false) {};

FbsfComponent FbsfInstantiate(QString fileName, int ac, char **av) {
    FbsfControllerComponent *comp = new FbsfControllerComponent(ac, av);
    if (!comp) {
        qInfo("Error: no instance");
        return comp;
    } else {
        comp->mu.lock();
        comp->configFileName = fileName;
        comp->mu.unlock();
    }
    if (comp->configFileName == "") {
        qWarning("Error: no configuration provided");
        return comp;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) || !file.size()) {
        qWarning("Error: configuration provided is invalid");
        return comp;
    }
    file.close();
    comp->threadRunning = true;
    comp->qtThread = std::thread(mainLoop, comp);
    while (1) {
        comp->mu.lock();
        if (!comp->threadRunning) {
            qInfo("Error: Initialisation mode is not  on");
            comp->mu.unlock();
            break;
        }
        if (comp->run == true && comp->app->executive()->State() != "initialized") {
            comp->mu.unlock();
            break;
        }
        comp->mu.unlock();
    }
    return comp;
};

void mainLoop(FbsfControllerComponent *comp) {
    // Create an app instance

    QScopedPointer<FbsfApplication> app(static_cast<FbsfApplication*>(mainApi(comp->ac, comp->av)));
    comp->app = app.data();
    // Retreive configuration name
    comp->app->config().Name() = comp->configFileName;
    if (!comp) {
        qFatal("Error: no instance");
    } else if (!comp->app) {
        qFatal("Error: no app");
    } else if (comp->app->configName() == "") {
        qWarning("Error: no configuration provided, use FbsfSetString first.");
    } else {
        // Open and parse XML config
        if(comp->app->config().parseXML(comp->app->configName())==-1) {
            qWarning("Error: can't parse xml configuration file provided: ", comp->app->configName().toStdString().c_str());
            comp->threadRunning = false;
            return;
        }
        // Generate sequences from XML infos
        comp->app->generateSequences();
    }
    // Wait for the FbsfExitInitialisationMode function to be called to start app run
    comp->mu.lock();
    comp->run = true;
    comp->mu.unlock();
    // Launch the qt app loop
    comp->app->start(comp->app->timeStep * 1000, comp->app->speedFactor, comp->app->recorderSize);
    comp->threadRunning = false;
}

FbsfSuccess FbsfDoStep(FbsfComponent ptr, int timeOut) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    FbsfStatus st;

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return Failure;
    }
    if (!comp->run) {
        qInfo("Error: App in not running, please exit initialisation mode");
        return Failure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: QT thread is not running, please enter initialisation mode");
        return Failure;
    }
    QString s = comp->app->executive()->State();
    if (s == "runnin" || s == "stepping") {
        qInfo("Error: A step is already running");
        return (Failure);
    }
    comp->app->executive()->control("step", QString::number(timeOut));
    FbsfGetStatus(ptr, &st);
    while (st == FbsfProcessing) {
        FbsfGetStatus(ptr,  &st);
    }
    return Success;
};

FbsfSuccess FbsfSaveState(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return Failure;
    }
    if (!comp->run) {
        qInfo("Error: App in not running, please exit initialisation mode");
        return Failure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: QT thread is not running, please enter initialisation mode");
        return Failure;
    }
    QString s = comp->app->executive()->State();
    if (s == "runnin" || s == "stepping") {
        qInfo("Error: Simulation is busy");
        return (Failure);
    }
    comp->app->executive()->control("saveLocal");
    return Success;
}

FbsfSuccess FbsfRestoreState(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return Failure;
    }
    if (!comp->run) {
        qInfo("Error: App in not running, please exit initialisation mode");
        return Failure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: QT thread is not running, please enter initialisation mode");
        return Failure;
    }
    QString s = comp->app->executive()->State();
    if (s == "runnin" || s == "stepping") {
        qInfo("Error: Simulation is busy");
        return (Failure);
    }
    comp->app->executive()->control("restoreLocal");
    return Success;
}

FbsfSuccess FbsfTerminate(FbsfComponent ptr) {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);

    if (!comp || !comp->app) {
        qInfo("Error: no instance");
        return Failure;
    }
    if (!comp->threadRunning) {
        qInfo("Error: The app is not running");
        if (comp->qtThread.joinable()) {
            comp->qtThread.join();
        }
        return Failure;
    }
//    QCoreApplication::exit(0);
    comp->app->executive()->control("stop");
//    comp->app->executive()->stopApp();
//    comp->qtThread.;
    if (comp->qtThread.joinable()) {
        comp->qtThread.join();
    }
    comp->isTerminated = true;
    return Success;
};

FbsfSuccess FbsfFreeInstance(FbsfComponent *ptr) {
    if (ptr && *ptr) {
        FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(*ptr);
        if (comp)
            delete comp;
        *ptr = nullptr;
    }
    return Success;
};
FbsfSuccess FbsfGetStatus(FbsfComponent ptr, FbsfStatus *value)  {
    FbsfControllerComponent *comp = static_cast<FbsfControllerComponent*>(ptr);
    if (!comp || !comp->app) {
        *value = FbsfUninitialized;
        return Success;
    }
    if (comp->configFileName == "") {
        *value = FbsfUninitialized;
        return Success;
    }
    if (comp->isTerminated) {
        *value = FbsfTerminated;
        return Success;
    }
    if (!comp->run) {
        *value = FbsfUninitialized;
        return Success;
    }
    if (!comp->threadRunning) {
        *value = FbsfUninitialized;
        return Success;
    }

    QString status = comp->app->executive()->State();
    if (status == "stepping" || status == "runnning" || status == "waiting") {
        *value = FbsfProcessing;
        return Success;
    } else {
        int exSt = comp->app->executive()->getStatus();
        switch (exSt) {
        case FBSF_OK:       *value = FbsfReady;break;
        case FBSF_WARNING:  *value = FbsfFailedStep;break;
        case FBSF_FATAL:    *value = FbsfFailedStep;break;
        case FBSF_ERROR:    *value = FbsfFailedStep;break;
        case FBSF_TIMEOUT:  *value = FbsfTimeOut;break;
        default:            *value = FbsfFailedStep;
        }
        return Success;
    }
    *value = FbsfReady;
    return Success;
};
FbsfSuccess FbsfGetRealData(FbsfComponent ptr, QString name, double *val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (data->Class() == cScalar && data->Type() == cReal) {
                *val = data->value().toDouble();
                return Success;
            } else if (data->Class() == cVector) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Vector Type";
            } else if (data->Type() == cInteger) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Integer Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while retreiving data, no data dound for that name "<< name;
    return Failure;
}

FbsfSuccess FbsfGetIntegerData(FbsfComponent ptr, QString name, int *val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (data->Class() == cScalar && data->Type() == cInteger) {
                *val = data->value().toInt();
                return Success;
            } else if (data->Class() == cVector) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Vector Type";
            } else if (data->Type() == cReal) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Real Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while retreiving data, no data dound for that name "<< name;
    return Failure;
}
FbsfSuccess FbsfGetVectorRealData(FbsfComponent ptr, QString name, QVector<double> *val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (data->Class() == cVector && data->Type() == cReal) {
                for (int i = 0; i < data->size(); i++) {
                    (*val)[i] = data->vectorValue(i).toDouble();
                }
                return Success;
            } else if (data->Class() == cScalar) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Scalar Type";
            } else if (data->Type() == cInteger) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Integer Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while retreiving data, no data dound for that name "<< name;
    return Failure;
}

FbsfSuccess FbsfGetVectorIntegerData(FbsfComponent ptr, QString name, QVector<int> *val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (data->Class() == cVector && data->Type() == cInteger) {
                for (int i = 0; i < data->size(); i++) {
                    (*val)[i] = data->vectorValue(i).toInt();
                }
                return Success;
            } else if (data->Class() == cScalar) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Scalar Type";
            } else if (data->Type() == cReal) {
                qInfo() << __FUNCTION__ << " Error while retreiving data, data is Real Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while retreiving data, no data dound for that name "<< name;
    return Failure;
}

FbsfSuccess FbsfSetRealData(FbsfComponent ptr, QString name, double val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (!data->isUnresolved()) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Vector Type";
                return Failure;
            }
            if (data->Class() == cScalar && data->Type() == cReal) {
                data->value((float)val);
                return Success;
            } else if (data->Class() == cVector) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Vector Type";
            } else if (data->Type() == cInteger) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Integer Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while setting data, no data dound for that name "<< name;
    return Failure;
}

FbsfSuccess FbsfSetIntegerData(FbsfComponent ptr, QString name, int val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (!data->isUnresolved()) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Vector Type";
                return Failure;
            }
            if (data->Class() == cScalar && data->Type() == cInteger) {
                data->value(val);
                return Success;
            } else if (data->Class() == cVector) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Vector Type";
            } else if (data->Type() == cReal) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Real Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while setting data, no data dound for that name "<< name;
    return Failure;
}
FbsfSuccess FbsfSetVectorRealData(FbsfComponent ptr, QString name, QVector<double> val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (!data->isUnresolved()) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Vector Type";
                return Failure;
            }
            if (data->Class() == cVector && data->Type() == cReal) {
                for (int i = 0; i < data->size(); i++) {
                    data->vectorValue(i, (val)[i]);
                }
                return Success;
            } else if (data->Class() == cScalar) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Scalar Type";
            } else if (data->Type() == cInteger) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Integer Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while setting data, no data dound for that name "<< name;
    return Failure;
}

FbsfSuccess FbsfSetVectorIntegerData(FbsfComponent ptr, QString name, QVector<int> val) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList)
    {
        // Register FbsfDataExchange for remote client
        if (data->name() == name) {
            if (!data->isUnresolved()) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Vector Type";
                return Failure;
            }
            if (data->Class() == cVector && data->Type() == cInteger) {
                for (int i = 0; i < data->size(); i++) {
                    data->vectorValue(i, (val)[i]);
                }
                return Success;
            } else if (data->Class() == cScalar) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Scalar Type";
            } else if (data->Type() == cReal) {
                qInfo() << __FUNCTION__ << " Error while setting data, data is Real Type";
            }
            return Failure;
        }
    }
    qInfo() << __FUNCTION__ << " Error while setting data, no data dound for that name "<< name;
    return Failure;
}

FbsfSuccess API_EXPORT FbsfGetDataNames(FbsfComponent ptr, QStringList *list) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    list->clear();
    foreach (FbsfDataExchange* data, dataList) {
        list->append(data->name());
    }
    return Success;
}

FbsfSuccess API_EXPORT FbsfGetUnresolvedDataNames(FbsfComponent ptr, QStringList *list) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    list->clear();
    foreach (FbsfDataExchange* data, dataList) {
        if (data->isUnresolved())
            list->append(data->name());
    }
    return Success;
}

FbsfSuccess API_EXPORT FbsfGetDataType(FbsfComponent ptr, QString name, FbsfDataType *type) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList) {
        if (data->name() == name) {
            if (data->Type() == cInteger)
                *type = data->Class() == cVector ? FbsfVectorInt : FbsfInt;
            else
                *type = data->Class() == cVector ? FbsfVectorReal : FbsfReal;
            return Success;
        }
    }
    qInfo() << __FUNCTION__ << " Error while retreiving data, no data dound for that name "<< name;
    return Failure;
}

FbsfSuccess API_EXPORT FbsfGetDataSize(FbsfComponent ptr, QString name, int *size) {
    QList<FbsfDataExchange*> dataList = FbsfDataExchange::sPublicDataMap.values();
    foreach (FbsfDataExchange* data, dataList) {
        if (data->name() == name) {
            *size = data->size();
            return Success;
        }
    }
    qInfo() << __FUNCTION__ << " Error while retreiving data, no data dound for that name "<< name;
    return Failure;
}

//                QStringList description;
    //            description.append(data->TypeToStr());
    //            description.append(data->producer());
    //            description.append(data->unit());
    //            description.append(data->description());
//                description.append(data->name());
                // Scalar or Vector
    //            description.append(data->ClassToStr());
    //            description.append(QString::number(data->size()));
