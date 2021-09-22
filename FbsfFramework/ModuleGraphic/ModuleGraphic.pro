#-------------------------------------------------
#
# Project created by QtCreator 2014-05-08T13:34:20
#
#-------------------------------------------------

#QT          -= gui
QT          += core qml quick xml
TARGET      = ModuleGraphic
TEMPLATE    = lib
CONFIG      += c++11

win32-g++{
    LIBS    += $$PWD/../../lib/FbsfBaseModel.a
}
win32-msvc* {
    LIBS    += $$PWD/../../lib/FbsfBaseModel.lib
}

#INCLUDEPATH += ../../FbsfFramework/src
INCLUDEPATH += $$PWD/../../FbsfBaseModel


DEFINES     += MODULE_GRAPHIC_LIBRARY

SOURCES     += \
    ModuleGraphic.cpp

HEADERS     +=\
    ModuleGraphic.h \
    ModuleGraphic_global.h

DESTDIR     = $$PWD/../../lib

OTHER_FILES +=

