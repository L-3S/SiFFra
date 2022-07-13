include($$(FBSF_HOME)/FbsfTargets.pri)
TEMPLATE=app
CONFIG  +=console

LIBS    += -L$$FBSF_LIB_DIR -lFbsfApi

DESTDIR =  $$PWD/../lib

INCLUDEPATH  += $$(FBSF_HOME)/FbsfFramework/FbsfApi
message($$(FBSF_HOME)/FbsfFramework/FbsfApi)
message("toto" $$FBSF_LIB_DIR)
SOURCES +=  \
    testapi.cpp


HEADERS +=  \
    testapi.h

