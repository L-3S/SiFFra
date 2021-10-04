TEMPLATE    = app

include(../FbsfTargets.pri)

# Configuration full batch code
BATCH {
    message( "Configuring FbsfFramework for BATCH build..." )
    TARGET  = FbsfBatch
    CONFIG  += console
    QT      += xml network
    DEFINES += MODE_BATCH
}
else {# Configuration gui code

    TARGET = FbsfFramework
    QT      += gui qml quick xml widgets network
    LIBS        += -L$$FBSF_HOME/Depends/ -lFbsfEditorLibrary
}

linux{
    QMAKE_LFLAGS += -rdynamic
}
win32-g++{
    QMAKE_LFLAGS += -Wl,--out-implib=$$FBSF_LIB_DIR/FbsfFramework.a
}
INCLUDEPATH += src FbsfPublicData FbsfNetwork
LIBS    += -L$$FBSF_LIB_DIR -lFbsfNetwork

LIBS    += -L$$FBSF_PLUGINS_DIR -lFbsfPublicData

# ADD DLL for FbsfBaseModel
LIBS        += -L$$FBSF_LIB_DIR -lFbsfBaseModel
INCLUDEPATH += $$FBSF_HOME/FbsfBaseModel
DEPENDPATH  += $$FBSF_HOME/FbsfBaseModel

INCLUDEPATH += shared/include shared/parser

DEFINES     += FBSF_FRAMEWORK_LIBRARY
DEFINES     += STANDALONE_XML_PARSER

include($$PWD/FbsfFrameworkSources.pri)

SOURCES +=  \


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




