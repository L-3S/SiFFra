TARGET = FbsfUtilities
TEMPLATE = lib

QT      += core qml quick xml widgets

CONFIG  += qt plugin c++11

DESTDIR = $$PWD

# export symbols
DEFINES += FBSF_UTILITIES

uri = FbsfUtilities

# Input
SOURCES += \
    FbsfUtilities_plugin.cpp \
    FbsfFileIO.cpp \
    FbsfMessageBox.cpp

HEADERS += \
    FbsfFileIO.h \
    FbsfUtilities_plugin.h \
    FbsfMessageBox.h
OTHER_FILES = qmldir

#--------------------------------------
INSTALLER_PATH  = $$PWD/../../Installer/package/Framework/data

target.path = $$INSTALLER_PATH/FbsfFramework/FbsfUtilities
qmldir.files += $$PWD/qmldir
qmldir.path = $$INSTALLER_PATH/FbsfFramework/FbsfUtilities


INSTALLS += target qmldir
