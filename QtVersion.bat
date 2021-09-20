rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem set QTDIR=C:\Qt
rem set QT_VERSION=5.15.0
rem set QT_COMPILATOR=msvc2019_64

if "%QTDIR%"=="" (
	echo Please defineQTDIR
	pause
	exit
)

if "%QT_VERSION%"=="" (
	echo Please define QTVERSION
	pause
	exit
)

if "%QT_COMPILER%"=="" (
	echo Please define QTVERSION
	pause
	exit
)

set PATH=%QTDIR%\%QT_VERSION%\%QT_COMPILER%\bin;%QTDIR%\Tools\QtCreator\bin;%PATH%
