import QtQuick 2.4
import QtGraphicalEffects 1.0
import "."

Item {
    id: probablityItem
    property alias probablity: mine.probablity
    property bool safe: false
    property bool opened: false
    property MobileCellCache cache

    width: mine.width; height: mine.height

    TrivialShader {
        id: mine
        property real probablity: 0.0
        source: cache.probabilitySource(mine.probablity);
        width: source.sourceItem.width; height: source.sourceItem.height
        anchors.centerIn: parent
        opacity: 1
    }
    
    Image {
        id: green
        anchors.fill: mine
        source: "green.png"
        opacity: 0
        visible: false
    }

    states: [
        State {
            name: "safe"
            when: probablityItem.safe
            PropertyChanges {
                target: mine
                probablity: 0
            }
            PropertyChanges {
                target: green
                visible: true
                opacity: 0.3
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
                    targets: [mine]
                    properties: "probablity"
                    easing.type: Easing.InOutQuad
                }

                NumberAnimation {
                    targets: [green]
                    properties: "opacity"
                    easing.type: Easing.InOutQuad
                }

                PropertyAction {
                    target: mine; property: "visible"
                }
            }
        }
    ]
}
