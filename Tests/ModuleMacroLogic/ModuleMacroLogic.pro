#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T13:34:20
#
#-------------------------------------------------
include($$(FBSF_HOME)/FbsfTargets.pri)

QT          += core qml quick xml
TARGET      = ModuleMacroLogic
TEMPLATE    = lib

LIBS        += $$FBSF_LIB_DIR/FbsfBaseModel.lib
INCLUDEPATH += $$(FBSF_HOME)/FbsfBaseModel

DEFINES     += MODULE_MACRO_LOGIC_LIBRARY

SOURCES     += \
    ModuleMacroLogic.cpp

HEADERS     +=\
    ModuleMacroLogic.h \
    ModuleMacroLogic_global.h

DESTDIR     = $$(APP_HOME)/lib

OTHER_FILES +=

