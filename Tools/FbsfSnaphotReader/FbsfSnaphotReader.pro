QT += core widgets
QT -= gui

CONFIG += c++11

TARGET = FbsfSnaphotReader
DESTDIR = $$(FBSF_HOME)/lib
INCLUDEPATH += $$(FBSF_HOME)/FbsfFramework/src
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
