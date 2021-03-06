include($$(FBSF_HOME)/FbsfTargets.pri)

TEMPLATE = app

QT += qml quick widgets xml
CONFIG += c++11

INCLUDEPATH +=$$(FBSF_HOME)/FbsfBaseModel\
              $$(FBSF_HOME)/FbsfFramework/src

LIBS        += $$FBSF_LIB_DIR/FbsfBaseModel.lib
RESOURCES   = qml.qrc
DEFINES     += FBSF_FRAMEWORK_LIBRARY # usefull for dllexport clause

SOURCES     = \
    ../../FbsfFramework/src/FbsfConfiguration.cpp \
    Controller.cpp \
    ItemParams.cpp \
    ModuleDescriptor.cpp \
    TreeItem.cpp \
    TreeModel.cpp \
    UndoManager.cpp \
    main.cpp

HEADERS += \
    ../../FbsfFramework/src/FbsfConfiguration.h \
    Controller.h \
    ItemParams.h \
    ItemProperties.h \
    ModuleDescriptor.h \
    TreeItem.h \
    TreeModel.h \
    UndoManager.h



