include(../../../FbsfTargets.pri)
TEMPLATE=app
QT      += gui qml quick xml widgets network

LIBS    += -L$$FBSF_LIB_DIR -lFbsfApi
LIBS    += -L$$FBSF_HOME/Depends/               -lFbsfEditorLibrary

INCLUDEPATH  += ../
DEPENDPATH  += ../
SOURCES +=  \
    testapi.cpp


HEADERS +=  \
    testapi.h
