#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T13:34:20
#
#-------------------------------------------------
include(../../FbsfTargets.pri)

#QT          -= gui
QT          += core qml quick xml
TARGET      = ModuleGraphic
TEMPLATE    = lib
CONFIG      += c++11

win32-g++{
    LIBS    += $$FBSF_LIB_DIR/FbsfBaseModel.a
}
win32-msvc* {
    LIBS    += $$FBSF_LIB_DIR/FbsfBaseModel.lib
}
#LIBS        += -L$$FBSF_LIB_DIR -lFbsfBaseModel


#INCLUDEPATH += ../../FbsfFramework/src
INCLUDEPATH += $$FBSF_HOME/FbsfBaseModel


DEFINES     += MODULE_GRAPHIC_LIBRARY

SOURCES     += \
    ModuleGraphic.cpp

HEADERS     +=\
    ModuleGraphic.h \
    ModuleGraphic_global.h

OTHER_FILES +=

