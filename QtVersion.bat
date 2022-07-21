@echo off

rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set QTDIR=C:\Qt
set QT_VERSION=5.15.0
set QT_COMPILER=msvc2019_64

if "%QTDIR%"=="" (
	if exist "C:\Qt\Qt5.15.0" (
		:: case of manual installer (ie on GREWP servers)
		set QTDIR=C:\Qt\Qt5.15.0
	) else if exist "C:\Qt" (
		:: case for online installer (ie on laptops)
		set QTDIR=C:\Qt
	) else (
		echo "Could not find Qt installation"
		echo "Please install Qt5.15.0 or define QTDIR manually"
	pause
	)
) 

if exist "%QTDIR%" (
	echo "- QTDIR is %QTDIR%"
) else (
	echo "QTDIR does not exist: %QTDIR%"
)

if "%QT_VERSION%"=="" (
	if exist "%QTDIR%\5.15.0" (
		set QT_VERSION=5.15.0
	) else (
		echo "Could not find Qt version 5.15.0"
		echo "Please install Qt5.15.0 or define QT_VERSION manually"
		pause
	)
)

if exist "%QTDIR%\%QT_VERSION%" (
	echo "- QTVERSION is %QT_VERSION%"
) else (
	echo "QTVERSION does not exist: %QT_VERSION%"
)

if "%QT_COMPILER%"=="" (
	if exist "%QTDIR%\%QT_VERSION%\msvc2015_64" (
		set QT_COMPILER=msvc2015_64
	) else if exist "%QTDIR%\%QT_VERSION%\msvc2019_64" (
		set QT_COMPILER=msvc2019_64
	) else (
		echo "Please define QT_COMPILER"
		pause
	) 
)

if exist "%QTDIR%\%QT_VERSION%\%QT_COMPILER%" (
	echo "- QT_COMPILER is %QT_COMPILER%"
) else (
	echo "QT_COMPILER does not exist: %QT_COMPILER%"
)

if exist "%QTDIR%\%QT_VERSION%\%QT_COMPILER%\bin" (
	echo "Using Qt at %QTDIR%\%QT_VERSION%\%QT_COMPILER%"
	set PATH=%QTDIR%\%QT_VERSION%\%QT_COMPILER%\bin;%QTDIR%\Tools\QtCreator\bin;%PATH%
) else (
	echo "Problem finding Qt at %QTDIR%\%QT_VERSION%\%QT_COMPILER%"
	pause
)

REM For newer versions of QtCreator
if exist "%QTDIR%\Tools\QtCreator\bin\jom" (
	set PATH=%QTDIR%\Tools\QtCreator\bin\jom;%PATH%
)	

