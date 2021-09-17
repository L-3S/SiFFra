copy /Y %QTDIR%\bin\Qt5Quick.dll                %1\lib
copy /Y %QTDIR%\bin\Qt5Qml.dll                  %1\lib
copy /Y %QTDIR%\bin\Qt5Network.dll              %1\lib
copy /Y %QTDIR%\bin\Qt5QuickControls2.dll    %1\lib
copy /Y %QTDIR%\bin\Qt5QuickTemplates2.dll  %1\lib
copy /Y %QTDIR%\bin\Qt5Core.dll                 %1\lib
copy /Y %QTDIR%\bin\icuin51.dll                 %1\lib
copy /Y %QTDIR%\bin\icuuc51.dll                 %1\lib
copy /Y %QTDIR%\bin\icudt51.dll                 %1\lib
copy /Y %QTDIR%\bin\Qt5Gui.dll                  %1\lib
copy /Y %QTDIR%\bin\libGLESv2.dll               %1\lib
copy /Y %QTDIR%\bin\Qt5Xml.dll                  %1\lib
copy /Y %QTDIR%\bin\libEGL.dll                  %1\lib
copy /Y %QTDIR%\bin\Qt5XmlPatterns.dll          %1\lib
copy /Y %QTDIR%\bin\Qt5Widgets.dll              %1\lib
copy /Y %QTDIR%\bin\Qt5Svg.dll                  %1\lib
copy /Y %QTDIR%\bin\Qt5QuickParticles.dll       %1\lib
copy /Y %QTDIR%\bin\D3DCompiler_43.dll          %1\lib

copy /Y %QTDIR%\qml\QtQuick.2\qtquick2plugin.dll                    %1\lib
copy /Y %QTDIR%\qml\QtQuick\XmlListModel\qmlxmllistmodelplugin.dll  %1\lib
copy /Y %QTDIR%\qml\QtQuick\Controls\qtquickcontrolsplugin.dll      %1\lib
copy /Y %QTDIR%\qml\QtQuick\Layouts\qquicklayoutsplugin.dll         %1\lib
copy /Y %QTDIR%\qml\QtQuick\Dialogs\dialogplugin.dll                %1\lib
copy /Y %QTDIR%\qml\QtQuick\Window.2\windowplugin.dll               %1\lib
copy /Y %QTDIR%\qml\QtQuick\Particles.2\particlesplugin.dll         %1\lib

xcopy /Y /E /I %QTDIR%\plugins\platforms                      %1\lib\platforms
copy /Y %QTDIR%\plugins\bearer\qgenericbearer.dll             %1\lib
copy /Y %QTDIR%\plugins\bearer\qnativewifibearer.dll          %1\lib
copy /Y %QTDIR%\plugins\imageformats\qgif.dll                 %1\lib
copy /Y %QTDIR%\plugins\imageformats\qico.dll                 %1\lib
copy /Y %QTDIR%\plugins\imageformats\qjpeg.dll                %1\lib
copy /Y %QTDIR%\plugins\imageformats\qmng.dll                 %1\lib
copy /Y %QTDIR%\plugins\imageformats\qsvg.dll                 %1\lib
copy /Y %QTDIR%\plugins\imageformats\qtga.dll                 %1\lib
copy /Y %QTDIR%\plugins\imageformats\qtiff.dll                %1\lib
copy /Y %QTDIR%\plugins\imageformats\qwbmp.dll                %1\lib

xcopy /Y /E /I %QTDIR%\qml\QtQuick                            %1\lib\QtQuick
xcopy /Y /E /I %QTDIR%\qml\QtQuick.2                          %1\lib\QtQuick.2
xcopy /Y /E /I %QTDIR%\qml\Qt\labs\folderlistmodel            %1\lib\Qt\labs\folderlistmodel
xcopy /Y /E /I %QTDIR%\qml\Qt\labs\settings                   %1\lib\Qt\labs\settings
