TEMPLATE    = app

# Configuration full batch code
BATCH {
    message( "Configuring FbsfFramework for BATCH build..." )
    # Production path
    DESTDIR = ../lib/batch
    TARGET  = FbsfBatch
    CONFIG  += console
    QT      += xml network
    DEFINES += MODE_BATCH
}
else {# Configuration gui code
    # Production path
    DESTDIR = ../lib
    TARGET = FbsfFramework
    QT      += gui qml quick xml widgets network
}

linux{
    QMAKE_LFLAGS += -rdynamic
}
win32-g++{
    QMAKE_LFLAGS += -Wl,--out-implib=../lib/FbsfFramework.a
}
INCLUDEPATH += src FbsfPublicData FbsfNetwork
LIBS    += -L$$PWD/../lib/ -lFbsfNetwork

LIBS    += -L$$PWD/fbsfplugins -lFbsfPublicData

# ADD DLL for FbsfBaseModel
LIBS        += -L$$PWD/../lib/ -lFbsfBaseModel
INCLUDEPATH += $$PWD/../FbsfBaseModel
DEPENDPATH  += $$PWD/../FbsfBaseModel

INCLUDEPATH += shared/include shared/parser
INCLUDEPATH += $$PWD/../Installer/LicenceManager

DEFINES     += FBSF_FRAMEWORK_LIBRARY
DEFINES     += STANDALONE_XML_PARSER

RESOURCES += resources.qrc
#RESOURCES += $$PWD/../FbsfEditors/GraphicEditor.qrc

SOURCES +=  \
            src/FbsfExecutive.cpp\
            src/FbsfPerfmeter.cpp \
            src/FbsfSequence.cpp\
            shared/parser/XmlElement.cpp \
            shared/parser/XmlParser.cpp \
            shared/parser/XmlParserCApi.cpp \
            src/FbsfApplication.cpp \
            src/FbsfControler.cpp \
            src/FbsfConfiguration.cpp \
            src/FbsfNode.cpp\
            src/main.cpp \
            ../Installer/LicenceManager/LicenseManager.cpp



HEADERS +=  \
            src/FbsfExecutive.h \
            src/FbsfPerfmeter.h \
            src/FbsfSequence.h\
            shared/parser/XmlElement.h \
            shared/parser/XmlParser.h \
            shared/parser/XmlParserCApi.h \
            shared/parser/XmlParserException.h \
            src/FbsfApplication.h \
            src/FbsfControler.h \
            src/FbsfConfiguration.h \
            src/FbsfNode.h
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Deployment target
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INSTALLER_PATH  = $$(FBSF_HOME)\Installer\package\Framework\data

#message($target.files)
target.path     =  $$INSTALLER_PATH
target.files    =  $$PWD/lib
target.depends  =  install_qtbinaries build_installer

config.path     =  $$INSTALLER_PATH
config.files    += $$PWD/FbsfEnv.bat
config.files    += $$PWD/plugins.xml
config.files    += $$PWD/ReleaseNotes.txt

dev.path        =  $$INSTALLER_PATH/include
dev.files       += $$PWD/FbsfBaseModel/FbsfBaseModel.h
dev.files       += $$PWD/FbsfFramework/src/FbsfBaseModelFMI.h
dev.files       += $$PWD/FbsfBaseModel/FbsfGlobal.h

fmu.path        =  $$INSTALLER_PATH/include
fmu.files       += $$PWD/FbsfFramework/shared/include/*.h
fmu.files       += $$PWD/FbsfBaseModel/FmuWrapper.h

library.path    =  $$INSTALLER_PATH
library.files   += $$PWD/Library

install_qtbinaries.commands = $$PWD/Installer/DeployQT.bat $$INSTALLER_PATH
build_installer.commands    = $$(FBSF_HOME)/Installer/BuildInstaller.bat

QMAKE_EXTRA_TARGETS +=  install_qtbinaries build_installer

INSTALLS += target config dev fmu library

