#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T13:34:20
#
#-------------------------------------------------
include($$(FBSF_HOME)/FbsfTargets.pri)

QT          -= gui

TARGET      = ModuleApiTest
TEMPLATE    = lib

LIBS    += $$FBSF_LIB_DIR/FbsfBaseModel.lib
INCLUDEPATH += $$(FBSF_HOME)/FbsfBaseModel

DEFINES     += ModuleApiTest_LIBRARY
DESTDIR     = $$PWD/../lib

SOURCES     += \
    ModuleApiTest.cpp

HEADERS     += \
    ModuleApiTest.h

OTHER_FILES += \
    ../testeur.xml \
    ../plugins.xml \
    ../DocGraphic/UIManual.qml

DISTFILES += \
    ../DocGraphic/UIManual.qml \
    ../TestBatch.xml \
    ../TestReplaySimu.xml \
    ../TestSimu.xml \
    ../TestSimuMPC.xml

