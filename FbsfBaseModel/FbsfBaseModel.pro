TEMPLATE = lib
DEFINES += FBSF_BASEMODEL_LIBRARY
CONFIG  += c++11
QT -= gui
QT += xml

DESTDIR     = $$(FBSF_HOME)/lib

DEFINES     += STANDALONE_XML_PARSER FMI_COSIMULATION

LIBS        += -L$$(FBSF_HOME)/FbsfFramework/fbsfplugins -lFbsfPublicData

INCLUDEPATH += ../FbsfFramework/FbsfPublicData
DEPENDPATH  += ../FbsfFramework/FbsfPublicData

INCLUDEPATH += ../FbsfFramework/src
DEPENDPATH  += ../FbsfFramework/src

INCLUDEPATH += ../FbsfFramework/shared/include ../FbsfFramework/shared/parser
DEPENDPATH  += ../FbsfFramework/shared/include ../FbsfFramework/shared/parser

SOURCES += \
    FbsfBaseModelFMI.cpp \
    FmuWrapper.cpp \
    ../FbsfFramework/shared/parser/XmlElement.cpp \
    ../FbsfFramework/shared/parser/XmlParser.cpp \
    ../FbsfFramework/shared/parser/XmlParserCApi.cpp \
    FbsfBaseModel.cpp


HEADERS += \
    FbsfBaseModelFMI.h \
    FbsfGlobal.h \
    FmuWrapper.h \
    ../FbsfFramework/shared/parser/XmlElement.h \
    ../FbsfFramework/shared/parser/XmlParser.h \
    ../FbsfFramework/shared/parser/XmlParserCApi.h \
    ../FbsfFramework/shared/parser/XmlParserException.h \
    FbsfBaseModel.h \    
    ParamProperties.h



# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

