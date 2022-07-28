include($$(FBSF_HOME)/FbsfTargets.pri)

TARGET      = ModuleMPC
TEMPLATE    = lib
QT          -= gui

LIBS        += $$FBSF_LIB_DIR/FbsfBaseModel.lib
INCLUDEPATH += $$(FBSF_HOME)/FbsfBaseModel

DEFINES     += ModuleMPC_LIBRARY

SOURCES     += \
    ModuleMPC.cpp

HEADERS     +=\
    ModuleMPC.h

DESTDIR     = $$PWD/../lib


