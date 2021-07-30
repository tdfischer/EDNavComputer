import QtQuick 2.0

Item {
    anchors.margins: 5
    Rectangle {
        id: background
        color: "#ff7100"
        opacity: 0.5
        anchors.fill: parent
    }
    default property alias content: container.children
    width: container.childrenRect.width + 10
    height: container.childrenRect.height + 10
    Item {
        anchors.margins: 5
        anchors.fill: parent
        id: container
    }
}
