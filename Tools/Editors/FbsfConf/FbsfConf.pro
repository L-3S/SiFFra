QT       += core gui qml quick xml widgets quickcontrols2
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 console
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES     += FBSF_FRAMEWORK_LIBRARY
DEFINES     += FMI_COSIMULATION STANDALONE_XML_PARSER

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cpp/LoaderType.cpp \
#    loadlib.cpp \
    cpp/ModuleManager.cpp \
    cpp/TypeManager.cpp \
    cpp/TypeModule.cpp \
    cpp/XmlApi.cpp \
    cpp/main.cpp
#    cpp/DirApi.cpp \

HEADERS += \
#    header/DirApi.h \
#    header/InfoModel.h \
    header/LoaderType.h \
    header/ModuleManager.h \
    header/NodeModule.h \
    header/TypeManager.h \
    header/TypeModule.h \
    header/Vector2S.h \
    header/XmlApi.h
#    loadlib.h \

FORMS += \
    mainwindow.ui

INCLUDEPATH += ./header

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    qml.qrc

MOC_DIR = ./src/moc
OBJECTS_DIR = ./src/objects
RCC_DIR = ./src/resource
UI_DIR = ./src/ui

LIBS        += -L../../FbsfFramework/fbsfplugins -lFbsfPublicData
INCLUDEPATH += $$PWD/../../FbsfFramework/FbsfPublicData
DEPENDPATH += $$PWD/../../FbsfFramework/FbsfPublicData

LIBS += -L$$PWD/../../../lib/ -lFbsfBaseModel
INCLUDEPATH += $$PWD/../../../FbsfBaseModel
DEPENDPATH += $$PWD/../../../FbsfBaseModel
