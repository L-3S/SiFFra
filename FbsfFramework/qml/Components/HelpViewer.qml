import QtQuick 2.0
import QtQuick.Window 2.0
import QtWebEngine 1.0
import QtWebView 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
Window
{
    id : win
    x:100;y:100
    width: 680
    height: 750
    visible: false
    property bool navigation:false
    property url displayurl
    ColumnLayout{anchors.fill: parent
        ToolBar {
            id: navigationBar
            Layout.preferredWidth: parent.width
            height: 16
            visible : navigation
            RowLayout {
                anchors.fill: parent
                spacing: 0

                ToolButton {
                    id: backButton
                    tooltip: qsTr("Back")
                    iconSource: "icons/left-32.png"
                    onClicked: webView.goBack()
                    enabled: webView.canGoBack
                    opacity : enabled ? 1:0.2
                    anchors.left: parent.left
                    style: ButtonStyle {
                        background: Rectangle { color: "transparent" }
                    }
                }

                ToolButton {
                    id: forwardButton
                    tooltip: qsTr("Forward")
                    iconSource: "icons/right-32.png"
                    onClicked: webView.goForward()
                    enabled: webView.canGoForward
                    opacity : enabled ? 1:0.2
                    anchors.right: parent.right
                    style: ButtonStyle {
                        background: Rectangle { color: "transparent" }
                    }
                }
            }
        }
        WebView {id: webView;
            Layout.preferredHeight: win.height- navigationBar.height
            Layout.preferredWidth: parent.width
            url:displayurl
        }

    }
    function display(url,withtoolbar)
    {
        var component = Qt.createComponent("HelpViewer.qml");
        if (component.status === Component.Ready)
        {
            var viewer = component.createObject(win,{title:"Help Viewer",
                                                    displayurl:url,
                                                    navigation:withtoolbar
                                                });
            if (viewer === null) console.log("Error creating object")
            else viewer.show();
        }
        else if (component.status === Component.Error) {
            console.log("Error loading component:", component.errorString());
        }
    }
}
