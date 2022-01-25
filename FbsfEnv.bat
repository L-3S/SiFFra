@echo off
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for framework
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if "%1" == "" (
	echo Defaut build mode is release
	set fbsfMode=release
) else (
	set fbsfMode=%1
)

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for framework
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set FBSF_HOME=%~dp0
set QML2_IMPORT_PATH=%FBSF_HOME%lib\%fbsfMode%
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set PATH=%FBSF_HOME%lib;%FBSF_HOME%lib\%fbsfMode%\;%FBSF_HOME%lib\%fbsfMode%\fbsfplugins;%FBSF_HOME%\Addons;%FBSF_HOME%\Depends;%PATH%

