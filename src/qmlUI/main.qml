import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Lazy minesweeper")

    Field {
        id: field
        anchors.fill: parent
        width: parent.width
        height: parent.height
    }

    footer: TabBar {
        id: tabBar
        TabButton {
            id: restartButton

            Behavior on text {
                SequentialAnimation {
                    id: playbanner
                    running: false
                    NumberAnimation { target: restartButton; property: "opacity"; to: 0.0; duration: 500}
                    PropertyAction {}
                    NumberAnimation { target: restartButton; property: "opacity"; to: 1.0; duration: 500}
                }
            }

            text: Field.solverRunning ? qsTr("Thinking...") : qsTr("Restart")
            onClicked: field.restart();
        }
    }
}
