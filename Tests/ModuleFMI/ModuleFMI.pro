include($$(FBSF_HOME)/FbsfTargets.pri)

QT          -= gui

TARGET      = ModuleFMI
TEMPLATE    = lib

LIBS        += $$FBSF_LIB_DIR/FbsfBaseModel.lib
INCLUDEPATH += $$(FBSF_HOME)/FbsfBaseModel

DEFINES     += ModuleFMI_LIBRARY
DESTDIR     = $$PWD/../lib

HEADERS = \
   $$PWD/ModuleFMI.h \
   $$PWD/ModuleFMI_global.h

SOURCES = \
   $$PWD/ModuleFMI.cpp

#DEFINES = 

