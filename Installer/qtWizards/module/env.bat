rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set QTDIR=E:\\tools\\Qt
set QT_VERSION=5.12.2
set PATH=%QTDIR%\\%QT_VERSION%\\msvc2017_64\\bin;%QTDIR%\\Tools\\QtCreator\\bin;%PATH%

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for Fbsf
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set FBSF_HOME=E:\\L3S\\9.Projets\\Sim\\fbsf\\
set PATH=%FBSF_HOME%\\lib;%FBSF_HOME%\\FbsfFramework\\fbsfplugins;%PATH%

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for Project
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set APP_HOME=%~dp0
set QML_IMPORT_PATH=%FBSF_HOME%/FbsfFramework;%FBSF_HOME%/FbsfEditors\GraphicEditor;%QML_IMPORT_PATH%

set PATH=%~dp0\\lib;%PATH%

IF "%1" == "" (
    start qtcreator.exe
) ELSE (
    start FbsfEditors.exe
)

