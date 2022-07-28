@echo off
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set PATH=""
call %~dp0..\QtVersion.bat
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for Project
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set APP_HOME=%~dp0

set PATH=%~dp0lib;%PATH%
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Target to run is simuSTD, simuMPC, batch
rem 
rem 
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if "%1" == "" (
	set fbsfMode=std
) else (
	set  fbsfMode=%1
)

if /i %fbsfMode% == std (
	echo mode standard
	call %~dp0..\fbsfenv.bat
	start FbsfFramework.exe TestSimu.xml
) else if /i %fbsfMode% == mpc (
	echo mode MPC
	call %~dp0..\fbsfenv.bat
	start FbsfFramework.exe TestSimuMPC.xml
) else if /i %fbsfMode% == batch (
	echo mode Batch
	call %~dp0..\fbsfenv.bat batch
	start cmd /K FbsfBatch.exe TestBatch.xml -no-logfile
	pause
)
