@echo off

cd %~dp0
IF NOT EXIST QtIFW2.0.3 echo "ERROR : Qt Installer Framework QtIFW2.0.3 not found"
set PATH=%FBSF_HOME%\Installer\QtIFW2.0.3\bin;%PATH%

binarycreator.exe --offline-only -p package -c config\config.xml FBSFInstaller
