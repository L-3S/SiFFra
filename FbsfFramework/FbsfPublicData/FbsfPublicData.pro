TEMPLATE = lib
TARGET = FbsfPublicData
DESTDIR = $$PWD/../fbsfplugins

# Configuration for gui code or full batch code
BATCH {
    message( "Configuring FbsfPublicData for BATCH build..." )
    DEFINES += MODE_BATCH
}
else {
    QT      += qml quick network
    CONFIG  += qt plugin c++11
}

# export symbols
DEFINES += FBSF_PUBLIC_DATA
#DEFINES += TRACE

uri = fbsfplugins

INCLUDEPATH += $$PWD/../../FbsfBaseModel

# Input
SOURCES += \
    FbsfPublicData.cpp \
    FbsfPublicData_plugin.cpp \
    FbsfQmlBinding.cpp \
    FbsfPublicDataModel.cpp \
    FbsfPublicDataRecorder.cpp

HEADERS +=  \
    FbsfPublicData.h \
    FbsfPublicData_plugin.h \
    FbsfQmlBinding.h \
    FbsfPublicDataModel.h \
    FbsfPublicDataRecorder.h

#-----------------------------------------------------
# Install target
#-----------------------------------------------------
INSTALLER_PATH  = $$PWD/../../Installer/package/Framework/data

target.path = $$INSTALLER_PATH/FbsfFramework/fbsfplugins

qmldir.files += $$PWD/qmldir $$PWD/plugins.qmltypes
qmldir.path = $$INSTALLER_PATH/FbsfFramework/fbsfplugins

INSTALLS += target qmldir


