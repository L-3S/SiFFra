@echo off

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Optional: Reseting minimal path
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set PATH=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
call %~dp0QtVersion.bat 

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Target to build is release,debug or batch
rem Could be an argument of this batch file or input from kewbord
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if "%1" == "" (
	echo A build mode is required. Please enter all, release, debug  or batch
	set /p fbsfMode=?:
) else (
	set  fbsfMode=%1
)
set FBSF_HOME=%~dp0

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for VISUAL compiler
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set CompilerPath="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %CompilerPath% (
	call %CompilerPath% x64
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"(
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
) else (
	echo CompilerPath Invalid
	pause
	exit
)

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem clean
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem if exist Makefile call jom clean

if exist Makefile call jom distclean 

if /i %fbsfMode% == all (
	echo Build FBSF all target 
	
	echo Build FBSF batch mode
	call qmake -recursive "CONFIG+=BATCH" FBSF.pro
	call jom
	
	call jom clean

	echo build fbsf release mode
	call qmake -recursive FBSF.pro
	call jom
	
	echo build FBSFConfig only release mode
	cd Tools\FBSFConfig
	call qmake FBSFConfig.pro
	call jom

	call jom clean

	echo Build FBSF debug mode	
	call qmake -recursive FBSF.pro
	call jom debug
		
) else if /i %fbsfMode% == release (
	echo Build FBSF release mode
	call qmake -recursive FBSF.pro
	call jom
	
) else if /i %fbsfMode% == debug (
	echo Build FBSF debug mode
	call qmake FBSF.pro
	call jom debug 
	
) else if /i %fbsfMode% == batch (
	echo Build FBSF batch mode
	call qmake "CONFIG+=BATCH" FBSF.pro
	call jom
	
) else (
	echo %fbsfMode% is not a build mode. Please enter release, debug, batch or all
)
pause
