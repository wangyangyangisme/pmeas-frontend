import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

import Theme 1.0

Window {
    id: settingsWindow
    title: qsTr( "Settings" );
    visible: false;
    x: root.x + ( ( root.width / 2 ) - ( width / 2 ) );
    y: root.y + ( ( root.height / 2 ) - ( height / 2 ) );
    width: 300;
    height: 250;
    modality: Qt.ApplicationModal;
    flags: Qt.Window;

    onVisibleChanged: {
        if(visible === true) {
            console.log("Getting ports")
            bridge.getPorts();
        }
    }

    ColumnLayout {
        id: devicesColumn;

        anchors { centerIn: parent; }
        spacing: 12;

        Text {
            text: "Audio Input";
        }

        ComboBox {
            id: inputs;
            model: bridge.inports;
            implicitWidth: 150;
        }

        Text {
            text: "Audio Output";
        }

        ComboBox {
            id: outputs;
            model: bridge.outports;
            implicitWidth: 150;
        }

    }

    RowLayout {
        anchors {
            top: devicesColumn.bottom;
            horizontalCenter: parent.horizontalCenter;
            margins: 12;
        }

        Rectangle {
            id: cancelSettings;
            anchors {
                right: sendSettings.left;
                rightMargin: 20;
            }
            width: 75;
            height: 35;
            color: "#FFFFFF";
            border.color: "#000000";
            border.width: 1;
            radius: 4;

            Text {
                anchors.centerIn: parent;
                text: qsTr("Cancel");
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    console.log("Cancel settings page");
                    settingsWindow.close();
                }

                onPressed: {
                    parent.color = "#E19854";
                }

                onReleased: {
                    parent.color = "#FFFFFF";
                }
            }
        }

        Rectangle {
            id: sendSettings;
            width: 75;
            height: 35;
            color: "#FFFFFF";
            border.color: "#000000";
            border.width: 1;
            radius: 4;

            Text {
                anchors.centerIn: parent;
                text: qsTr("Accept");
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    bridge.sendPorts();
                }

                onPressed: {
                    parent.color = "#E19854";
                }

                onReleased: {
                    parent.color = "#FFFFFF";
                }
            }
        }
    }
}
