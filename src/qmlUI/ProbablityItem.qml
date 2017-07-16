import QtQuick 2.4
import QtGraphicalEffects 1.0
import "."

Item {
    id: probablityItem
    property alias probablity: mine.probablity
    property bool safe: false
    property bool opened: false
    property MobileCellCache cache

    TrivialShader {
        id: mine
        visible: !safe
        property real probablity: 0.0
        source: cache.probabilitySource(mine.probablity);
        width: source.sourceItem.width; height: source.sourceItem.height
        anchors.centerIn: parent
        opacity: 1
    }
    
    Image {
        id: green
        anchors.fill: parent
        source: "green.png"
        opacity: 0
        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }
    }

    states: [
        State {
            name: "safe"
            when: probablityItem.safe
            PropertyChanges {
                target: mine
                visible: false
                opacity: 0
            }
            PropertyChanges {
                target: green
                visible: true
                opacity: 0.2
            }
        }
    ]

    transitions: [
        Transition {
            to: "safe"
            reversible: true
            SequentialAnimation {
                PropertyAction {
                    target: green; property: "visible"
                }

                NumberAnimation {
                    targets: [green, mine]
                    properties: "opacity"
                    duration: 200
                }

                PropertyAction {
                    target: mine; property: "visible"
                }
            }
        }
    ]
}
