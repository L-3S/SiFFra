TARGET = FbsfNetwork
TEMPLATE = lib
CONFIG      += c++11

QT += network

# export symbols
DEFINES     += FBSF_NETWORK
INCLUDEPATH += ../../FbsfFramework/src

# Build path
DESTDIR = $$PWD/../../lib

HEADERS = \
    FbsfNetServer.h \
    FbsfTcpServer.h \
    FbsfNetProtocol.h \
    FbsfNetLogger.h \
    FbsfSrvClient.h \
    FbsfNetGlobal.h \
    FbsfNetClient.h

SOURCES = \
    FbsfNetServer.cpp \
    FbsfTcpServer.cpp \
    FbsfNetProtocol.cpp \
    FbsfNetLogger.cpp \
    FbsfSrvClient.cpp \
    FbsfNetClient.cpp

#----------------------------------------------------
INSTALLER_PATH  = $$PWD/../../Installer/package/Framework/data
target.path =   $$INSTALLER_PATH/lib
INSTALLS += target
