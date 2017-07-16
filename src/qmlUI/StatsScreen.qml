import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: statsScreen
    property Item source
    property bool win: false
    property bool shown: false
    signal restart();

    visible: false
    anchors.fill: source

    state: shown ? "shown" : ""
    
    FastBlur {
        id: background
        anchors.fill: parent
        source: statsScreen.source
        radius: 0
    }


    // to disable click trough
    MouseArea {
        anchors.fill: parent
    }

    Item {
        id: dialog
        width: content.width + 40; height: content.height + 40;
        anchors.bottom: parent.top
        anchors.horizontalCenter: statsScreen.horizontalCenter
        anchors.verticalCenter: undefined

        Rectangle {
            color: "#80000000"
            anchors.fill: dialog
        }

        ColumnLayout {
            id: content
            anchors.centerIn: parent
            spacing: text.height

            Text {
                id: text
                text: win
                      ? qsTr("We won!")
                      : qsTr("Game over")
                color: "white"
                font.pixelSize: statsScreen.width/15
            }

            Button {
                text: qsTr("Start new game");
                anchors.horizontalCenter: text.horizontalCenter
                onClicked: statsScreen.restart()
            }
        }
    }

    states: [
        State {
            name: "shown"
            AnchorChanges {
                target: dialog
                anchors.verticalCenter: statsScreen.verticalCenter
                anchors.bottom: undefined
            }
            PropertyChanges {
                target: background
                radius: win ? 10 : 50
            }
            PropertyChanges {
                target: statsScreen
                visible: true
            }
        }
    ]

    transitions: [
        Transition {
            to: "shown"
            SequentialAnimation {
                PropertyAction {
                    target: statsScreen; property: "visible"
                }
                ParallelAnimation {
                    AnchorAnimation {
                        targets: [dialog]
                        duration: 500
                        easing.type: Easing.InOutQuad
                    }

                    NumberAnimation {
                        target: background
                        property: "radius"
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
    ]
}