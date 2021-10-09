@echo off
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Target to build is release,debug or batch
rem Could be an argument of this batch file or input from kewbord
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if "%1" == "" (
	echo A build mode is required. Please enter release, debug  or batch
	set /p fbsfMode=?:
) else (
	set /p fbsfMode = %1
)

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
call %~dp0QtVersion.bat 

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for VISUAL compiler
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set CompilerPath="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %CompilerPath% (
	call %CompilerPath% x64
) else (
	echo CompilerPath Invalid
	pause
	exit
)

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem clean
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem if exist Makefile call jom distclean


if /i %fbsfMode% == release (
	echo build fbsf release mode
	call qmake FBSF.pro
	call jom
	
) else if /i %fbsfMode% == debug (
	echo build fbsf debug mode
	call qmake FBSF.pro
	call jom debug 
) else if /i %fbsfMode% == batch (
	echo build fbsf batch mode
	call qmake "CONFIG+=BATCH" FBSF.pro
	call jom
) else (
	echo %fbsfMode% is not a build mode. Please enter release, debug or batch
)
pause
