import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
MenuBar {
    id:menuBar
        background: Rectangle {
            anchors.fill: parent
            color: style.topMenuColor
        }
        Menu {
            title: "File"

            MenuItem {
                text: "Load XML"
                onTriggered: fileDialogxml.open();
            }
            MenuItem {
                text: "Load Module"
                onTriggered: fileDialogDll.open();
            }
            MenuItem {
                text: "Save"
                onTriggered: xmlapi.save(spaceManager);
            }
        }
        Menu {
            title: "Edit"
            MenuItem {
                text: "Copy"
            }
            MenuItem {
                text: "Paste"
            }
        }
        Menu {
            title: "Help"
            MenuItem {
                text: "ALED"
            }
        }

        function createSpriteObjects(savcomponent, path) {
            var component;
            var sprite;
            if (savcomponent !== "") {
                savcomponent.destroy();
            }

            component = Qt.createComponent("LeftMenu.qml");
            savcomponent = component.createObject(windowSimulation, {nameDir: path});

            if (sprite === null) {
                // Error Handling
                console.log("Error creating object");
            }
            return savcomponent;
        }

        FileDialog {
                id: fileDialogDll
                onAccepted: {
                    if (fileDialogDll.selectExisting) {
                        var path = fileUrl.toString();
                         // remove prefixed "file:///"
                         path = path.replace(/^(file:\/{3})/,"");
                         // unescape html codes like '%23' for '#'
                         var cleanPath = "/" + decodeURIComponent(path);
                        console.log(fileUrl);
                         console.log(cleanPath)
                          loadlib(cleanPath);

                    }
                    else
                        console.log("Error " + fileUrl)
                }
                selectFolder: false
                selectMultiple: false
                selectExisting: true
                nameFilters: [ "All files (*.so *.dll)" ]

            }

        FileDialog {
                id: fileDialogxml
                onAccepted: {
                    if (fileDialogxml.selectExisting) {
                        var path = fileUrl.toString();
                         // remove prefixed "file:///"
                         path = path.replace(/^(file:\/{3})/,"");
                         // unescape html codes like '%23' for '#'
                         var cleanPath = "/" + decodeURIComponent(path);
                        console.log(fileUrl);
                         console.log(cleanPath)
                          preload(cleanPath);

                    }
                    else
                        console.log("Error " + fileUrl)
                }
                selectFolder: false
                selectMultiple: false
                selectExisting: true
                nameFilters: [ "All files (*.xml)" ]

            }
        Action {
                id: fileOpenAction
                text: "Open"
                onTriggered: {
                    fileDialog.selectExisting = true
                    fileDialog.open()
                }
            }

    }

