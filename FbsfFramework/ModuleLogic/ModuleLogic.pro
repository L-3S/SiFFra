#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T13:34:20
#
#-------------------------------------------------

#QT          -= gui
QT          += core qml quick xml
TARGET      = ModuleLogic
TEMPLATE    = lib
CONFIG      += c++11

win32-g++{
    LIBS    += $$PWD/../../lib/FbsfBaseModel.a
}
win32-msvc* {
    LIBS    += $$PWD/../../lib/FbsfBaseModel.lib
}

INCLUDEPATH += ../../FbsfBaseModel
#INCLUDEPATH += ../../FbsfFramework/src
INCLUDEPATH += ../../FbsfFramework/FbsfPublicData

DEFINES     += MODULE_LOGIC_LIBRARY

SOURCES     += \
    ModuleLogic.cpp

HEADERS     +=\
    ModuleLogic.h \
    ModuleLogic_global.h

DESTDIR     = $$PWD/../../lib

OTHER_FILES +=

