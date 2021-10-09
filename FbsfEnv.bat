@echo off
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for framework
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if "%1" == "" (
	echo Defaut build mode is release
	set /p fbsfMode= release
) else (
	set /p fbsfMode = %1
)

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for framework
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set FBSF_HOME=%~dp0

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set PATH=%FBSF_HOME%lib;%FBSF_HOME%lib\%fbsfMode%\;%FBSF_HOME%\FbsfFramework\fbsfplugins;%FBSF_HOME%\Addons;%FBSF_HOME%\Depends;%PATH%

