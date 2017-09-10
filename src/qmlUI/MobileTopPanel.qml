import QtQuick 2.4
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

import "timeFormat.js" as TimeFormatModule

Item {
    id: topPanel
    height: 40
    property int bombRemains: 99
    property MobileCellCache cache
    property alias solverInprogress: solverButton.inProgress
    property alias solverActive: solverButton.active

    property int secocndsHumanSpent: 0
    property int secocndsMachineSpent: 0
    property real riskTaken: 0

    signal makeBestTurn();
    signal restart();


    Behavior on bombRemains {
        NumberAnimation {}
    }
    
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    
    ColorOverlay {
        id: bgDarkner
        anchors.fill: parent
        source: ShaderEffectSource {
            sourceItem: fieldContainer
            sourceRect: mapToItem(fieldContainer.background, 0, 0, topPanel.width, topPanel.height)
        }
        color: "#10000000"
        visible: false
    }
    
    FastBlur {
        anchors.fill: parent
        radius: 32
        source: bgDarkner
        //visible: false
    }
    
    Row {
        spacing: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        GlowText {
            id: bombRemains
            width: 30;
            anchors.verticalCenter: parent.verticalCenter
            text: topPanel.bombRemains
        }

//        ProbablityItem {
//            safe: false
//            anchors.verticalCenter: parent.verticalCenter
//            cache: topPanel.cache
//            probablity: riskTaken

//            Behavior on probablity {NumberAnimation {
//                    easing.type: Easing.InOutQuad
//                }
//            }
//        }
    }

    Row {
        anchors.right: parent.right
        spacing: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Button {
            id: restartButton

            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 4

            text: "Restart"
            onClicked: restart();
        }
    }

    Row {
        spacing: 5
        anchors.left: parent.left
        height: parent.height
        Button {
            id: solverButton

            property bool inProgress: false
            property bool active: true

            anchors.verticalCenter: parent.verticalCenter
            width: parent.height; height: parent.height

            indicator: Item {
                anchors.fill: parent

                Rectangle {
                    color: "#f0f0f0"
                    anchors.fill: parent
                    opacity: 0.8;
                }
                ProbablityItem {
                    id: trivialShader
                    anchors.fill: parent
                    anchors.margins: 3
                    safe: solverButton.active && !solverButton.inProgress
                    cache: topPanel.cache
                    SequentialAnimation {
                        loops: Animation.Infinite
                        running: solverButton.inProgress
                        NumberAnimation {
                            target: trivialShader
                            property: "probablity"
                            from: 0.0; to: 1.0;
                            duration: 1000
                        }
                        NumberAnimation {
                            target: trivialShader
                            property: "probablity"
                            to: 0.0;
                            duration: 1000
                        }
                    }
                }
            }
        }

//        GlowText {
//            anchors.verticalCenter: parent.verticalCenter
//            text: TimeFormatModule.toHHMMSS(secocndsMachineSpent)
//            font.pixelSize: 15
//        }

        GlowText {
            anchors.verticalCenter: parent.verticalCenter
            text: TimeFormatModule.toHHMMSS(secocndsHumanSpent)
            font.pixelSize: 15
        }

    }
}
