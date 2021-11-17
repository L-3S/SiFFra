#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T13:34:20
#
#-------------------------------------------------
include(../../FbsfTargets.pri)

#QT          -= gui
QT          += core qml quick xml
TARGET      = ModuleLogic
TEMPLATE    = lib
CONFIG      += c++11

win32-g++{
    LIBS    += $$FBSF_LIB_DIR/FbsfBaseModel.a
}
win32-msvc* {
    LIBS    += $$FBSF_LIB_DIR/FbsfBaseModel.lib
}

INCLUDEPATH += $$FBSF_HOME/FbsfBaseModel
#INCLUDEPATH += ../../FbsfFramework/src
INCLUDEPATH += $$FBSF_FRAMEWORK_DIR/FbsfPublicData

DEFINES     += MODULE_LOGIC_LIBRARY

SOURCES     += \
    ModuleLogic.cpp

HEADERS     +=\
    ModuleLogic.h \
    ModuleLogic_global.h


OTHER_FILES +=

