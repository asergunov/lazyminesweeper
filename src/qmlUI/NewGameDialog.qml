import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: statsScreen
    property Item source

    property bool shown: false
    signal restart(size size, int bombCount);

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

            Repeater {
                model: ListModel {
                    Component.onCompleted: {
                        append({"name": qsTr("Begginer"), "size": Qt.size(8,8), "bombCount": 10});
                        append({"name": qsTr("Master"), "size": Qt.size(16,16), "bombCount": 40});
                        append({"name": qsTr("Expert"), "size": Qt.size(25,25), "bombCount": 99});
                    }
                }

                delegate: Button {
                    Layout.fillWidth: true
                    text: name + " " + model.size.width + "x" + model.size.height + " bombs: " + model.bombCount
                    anchors.horizontalCenter: text.horizontalCenter
                    onClicked: statsScreen.restart(Qt.size(model.size.width, model.size.height), model.bombCount);
                }
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
                radius: 10
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
