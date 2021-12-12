TEMPLATE    = lib
# Le config mode est il interne ???
# si j'ai compris faut se trimballer le thread
#le main est ds un autre proj

#BUILD_API {
include(../../FbsfTargets.pri)

BATCH {
    message( "Configuring FbsfFramework for BATCH build..." )
    TARGET = FbsfApi
    CONFIG  += console
    QT      += xml network
    DEFINES += MODE_BATCH
}
else {# Configuration gui code

    TARGET = FbsfApi
    QT      += gui qml quick xml widgets network
    LIBS        += -L$$FBSF_HOME/Depends/ -lFbsfEditorLibrary
}

DEFINES     += STANDALONE_XML_PARSER
DEFINES     += FBSF_FRAMEWORK_LIBRARY
DEFINES += BUILD_API
message("build api")

INCLUDEPATH += $$FBSF_FRAMEWORK_DIR/FbsfPublicData\
               $$FBSF_FRAMEWORK_DIR/FbsfNetwork

LIBS    += -L$$FBSF_LIB_DIR         -lFbsfNetwork
LIBS    += -L$$FBSF_PLUGINS_DIR     -lFbsfPublicData
LIBS    += -L$$FBSF_LIB_DIR         -lFbsfBaseModel

# ADD DLL for FbsfBaseModel
DEPENDPATH  += $$FBSF_HOME/FbsfBaseModel

INCLUDEPATH += $$FBSF_HOME/FbsfBaseModel
INCLUDEPATH += $$FBSF_FRAMEWORK_DIR/src
INCLUDEPATH += shared/include shared/parser



#add all FbsfFramework Sources
include($$FBSF_FRAMEWORK_DIR/FbsfFrameworkSources.pri)

SOURCES +=  \
            FbsfApi.cpp\

HEADERS +=  \
            FbsfApi.h \

#Set dest



