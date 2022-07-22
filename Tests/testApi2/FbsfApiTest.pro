include(../../FbsfTargets.pri)
TEMPLATE=app
CONFIG  +=console

QT += testlib

LIBS    += -L$$FBSF_LIB_DIR -lFbsfApi

INCLUDEPATH  += $$(FBSF_HOME)/FbsfFramework/FbsfApi
message($$(FBSF_HOME)/FbsfFramework/FbsfApi)
SOURCES +=  \
    qtestapi.cpp


HEADERS +=  \
    testapi.h


