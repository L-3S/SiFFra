QT       -= gui
CONFIG +=  c++11

TARGET = %{ProjectName}

TEMPLATE = lib
DEFINES += %{ProjectName}_LIBRARY
win32-g++{
    LIBS    += $$(FBSF_HOME)/lib/FbsfFramework.a
}
win32-msvc* {
    LIBS    += $$(FBSF_HOME)/lib/FbsfFramework.lib
}
INCLUDEPATH += $$(FBSF_HOME)/FbsfFramework/src

DESTDIR     = $$(APP_HOME)/lib

SOURCES += \\
        %{CppFileName}

HEADERS += \\
        %{HdrFileName} \\
        %{GlbHdrFileName} 
		
OTHER_FILES += \\
		simu.xml \\
		env.bat
		