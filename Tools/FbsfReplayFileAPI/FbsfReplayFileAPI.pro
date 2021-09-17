QT -= gui
QT += quick
CONFIG += c++11

TARGET = FbsfReplayFileAPI
TEMPLATE = lib

DESTDIR = $$(FBSF_HOME)/lib

INCLUDEPATH += $$(FBSF_HOME)/FbsfFramework/src
INCLUDEPATH += $$(FBSF_HOME)/FbsfFramework/FbsfPublicData
LIBS        += $$(FBSF_HOME)/FbsfFramework/fbsfplugins/FbsfPublicData.lib

DEFINES += FBSFREPLAYFILEAPI_LIBRARY

SOURCES += \
        FbsfReplayFileAPI.cpp
HEADERS += \
        FbsfReplayFileAPI.h

SUBDIRS += \
    TestAPI.pro
