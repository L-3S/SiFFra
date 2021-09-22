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
set APP_HOME=%FBSF_HOME%/Tests/TEST-FBSF
set PATH=%FBSF_HOME%/lib;%APP_HOME%/lib;%PATH%

IF "%1" == "" (
    start qtcreator.exe FBSFConfig.pro
) ELSE (
    start %1
)

