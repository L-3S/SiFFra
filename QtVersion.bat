rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rem Path for QT toolkit
rem ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set QTDIR=C:\Qt
set QT_VERSION=5.15.0
set QT_COMPILER=msvc2019_64

if "%QTDIR%"=="" (
	echo Please define QTDIR
	pause
	exit
)

if "%QT_VERSION%"=="" (
	echo Please define QT_VERSION
	pause
	exit
)

if "%QT_COMPILER%"=="" (
	echo Please define QT_COMPILER
	pause
	exit
)

set PATH=%QTDIR%\%QT_VERSION%\%QT_COMPILER%\bin;%QTDIR%\Tools\QtCreator\bin;%PATH%
