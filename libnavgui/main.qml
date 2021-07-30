import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Particles 2.12

Item {
    anchors.fill: parent
    visible: true
    //title: qsTr("Hello World")

    DisplayPanel {
        id: navbar
        anchors.left: parent.left
        anchors.right: parent.right
        ColumnLayout {
            anchors.left: parent.left
            anchors.top: parent.top
            DisplayPanel {
                Text {
                    text: "System: " + Journal.location.system;
                }
            }

            DisplayPanel {
                Text {
                    text: "Station: " + Journal.location.station;
                }
            }
        }
        Button {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: "Load journal!"
            onClicked: Journal.load();
        }
    }

    ListView {
        anchors.top: navbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5

        model: Journal.exploration.bodies;
        spacing: 5
        clip: true
        delegate: DisplayPanel {
            width: parent ? parent.width : childrenRect.width
            ColumnLayout {
                anchors.left: parent.left
                Text {
                    text: name
                    font.pointSize: 18
                }
                RowLayout {
                    Text {
                        text: starType + planetClass
                    }
                    Text {
                        text: "Scan value: " + scanValue
                    }
                }
            }

            function colorForStarClass(starClass) {
                if (starClass === "O") {
                    return "#9bb0ff"
                } else if (starClass === "B") {
                    return "#aabfff"
                } else if (starClass === "A") {
                    return "#cad7ff"
                } else if (starClass === "F") {
                    return "#f8f7ff"
                } else if (starClass === "G") {
                    return "#fff4ea"
                } else if (starClass === "K") {
                    return "#ffd2a1"
                } else if (starClass === "M") {
                    return "#ffcc6f"
                }
                return "#ffcc6f"
            }

            Rectangle {
                border.width: 2
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: height
                color: colorForStarClass(starType)

                Text {
                    anchors.centerIn: parent
                    text: starType
                    font.pointSize: 15
                }
            }
        }
    }
}
