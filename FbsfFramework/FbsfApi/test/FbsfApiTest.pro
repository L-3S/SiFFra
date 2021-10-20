include(../../../FbsfTargets.pri)
TEMPLATE=app
CONFIG  +=console

LIBS    += -L$$FBSF_LIB_DIR -lFbsfApi

INCLUDEPATH  += ../
DEPENDPATH  += ../
SOURCES +=  \
    testapi.cpp


HEADERS +=  \
    testapi.h

