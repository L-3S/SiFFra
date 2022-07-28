#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T13:34:20
#
#-------------------------------------------------
include($$(FBSF_HOME)/FbsfTargets.pri)

QT          -= gui

TARGET      = ModuleMini
TEMPLATE    = lib

LIBS    += $$FBSF_LIB_DIR/FbsfBaseModel.lib
INCLUDEPATH += $$(FBSF_HOME)/FbsfBaseModel

DEFINES     += ModuleMini_LIBRARY
DESTDIR     = $$PWD/../lib

SOURCES     += \
    ModuleMini.cpp

HEADERS     += \
    ModuleMini.h

OTHER_FILES += \
    ../testeur.xml \
    ../plugins.xml \


