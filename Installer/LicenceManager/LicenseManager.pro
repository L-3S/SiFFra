#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T11:24:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = keygen
TEMPLATE = app


SOURCES += main.cpp\
            mainwindow.cpp \
            LicenseManager.cpp

HEADERS  += mainwindow.h \
            LicenseManager.h

FORMS    += mainwindow.ui
