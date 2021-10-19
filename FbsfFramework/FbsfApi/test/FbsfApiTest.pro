include(../../../FbsfTargets.pri)
TEMPLATE=app
QT      += gui qml quick xml widgets network
CONFIG  +=console
LIBS    += -L$$FBSF_LIB_DIR -lFbsfApi
LIBS    += -L$$FBSF_HOME/Depends/               -lFbsfEditorLibrary

INCLUDEPATH  += ../
DEPENDPATH  += ../
SOURCES +=  \
    testapi.cpp


HEADERS +=  \
    testapi.h

DISTFILES += \
    main.qml

RESOURCES += \
    ressources.qrc
