#-------------------------------------------------
#
#-------------------------------------------------
include(../../FbsfTargets.pri)

TEMPLATE    = lib
QT          -= gui
TARGET      = FbsfTimeManager
CONFIG      += c++11

LIBS        += -L$$FBSF_PLUGINS_DIR -lFbsfPublicData
INCLUDEPATH += ../FbsfPublicData
DEPENDPATH  += ../FbsfPublicData
INCLUDEPATH += $$FBSF_HOME/FbsfBaseModel


DEFINES     +=FBSF_TIMEMANAGER_LIBRARY

SOURCES     += FbsfTimeManager.cpp

HEADERS     += FbsfTimeManager.h


OTHER_FILES +=

