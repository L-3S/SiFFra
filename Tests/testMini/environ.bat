rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
call %~dp0..\..\QtVersion.bat
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for framework
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
call %~dp0..\..\fbsfenv.bat
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for Project
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set APP_HOME=%~dp0
set QML_IMPORT_PATH=%FBSF_HOME%/FbsfFramework;%FBSF_HOME%/FbsfEditors\GraphicEditor;%QML_IMPORT_PATH%

set PATH=%~dp0lib;%PATH%

IF "%1" == "" (
    start qtcreator.exe
) ELSE (
    start FbsfEditors.exe
)

