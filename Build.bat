@echo off
REM for RELEASE set OPTION=release
REM for debug set OPTION=debug
REM OPTION must be defined by calling script
REM Or OPTION is defined by user in interactive mode (detected by void option)
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
call %~dp0QtVersion.bat
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for framework
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
call %~dp0fbsfenv.bat

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for VISUAL compiler
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set CompilerPath = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
IF EXIST %CompilerPath" (
	call CompilerPath
) ELSE (
	ECHO CompilerPath Invalid
	pause
	exit
)

set build_config=""
if /i "%1" == "BATCH" (
set build_config="CONFIG+=BATCH"
) else (
ECHO A build mode is required. please enter release or debug
set /p OPTION=release or debug?:
)

REM clean
if exist Makefile call jom distclean


REM Create makefiles
call qmake  %build_config% FBSF.pro


REM compile
if %OPTION%==debug (call jom debug) else (call jom)
