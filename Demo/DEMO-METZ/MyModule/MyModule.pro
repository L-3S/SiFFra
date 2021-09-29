#-------------------------------------------------
#
# Project created by QtCreator 2019-08-05T16:26:37
#
#-------------------------------------------------

QT       -= gui

TARGET = MyModule
TEMPLATE = lib

DEFINES += MYMODULE_LIBRARY

win32-g++{
    LIBS    += $$(FBSF_HOME)/lib/FbsfFramework.a
}
win32-msvc* {
    LIBS    += $$(FBSF_HOME)/lib/FbsfFramework.lib
}

#Should check the Fbsf instalation for the include files of Fbsf
INCLUDEPATH += $$(FBSF_HOME)/FbsfFramework/src

DESTDIR     = $$(APP_HOME)/lib

SOURCES += \
        mymodule.cpp

HEADERS += \
        mymodule.h \
        mymodule_global.h 

unix {
    target.path = /usr/lib
    INSTALLS += target
}
