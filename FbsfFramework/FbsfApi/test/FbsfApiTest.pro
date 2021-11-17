include(../../../FbsfTargets.pri)
TEMPLATE=app
CONFIG  +=console

LIBS    += -L$$FBSF_LIB_DIR -lFbsfApi

INCLUDEPATH  += $$(FBSF_HOME)/FbsfFramework/FbsfApi

SOURCES +=  \
    testapi.cpp


HEADERS +=  \
    testapi.h

