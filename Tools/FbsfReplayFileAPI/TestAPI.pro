QT += core
#QT -= gui

CONFIG += c++11

TARGET = testAPI
DESTDIR = $$(PWD)
LIBS    += $$(FBSF_HOME)/lib/FbsfReplayFileAPI.lib

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
