include($$PWD/FbsfTargets.pri)
TEMPLATE = subdirs
CONFIG += ordered qtquickcompiler
SUBDIRS = \
          FbsfFramework/FbsfPublicData\
          FbsfFramework/FbsfUtilities\
          FbsfFramework/FbsfNetwork\
          FbsfBaseModel\
          FbsfFramework\
          FbsfFramework/ModuleLogic\
          FbsfFramework/ModuleGraphic\
          FbsfFramework/FbsfApi\
