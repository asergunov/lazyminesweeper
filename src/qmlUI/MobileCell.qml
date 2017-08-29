import QtQuick 2.4
import QtGraphicalEffects 1.0
import "."

Item {
    id:cell
    property int nearBombCount: 4
    property bool flaged : false
    property bool safe : false
    property bool opened : false
    property bool exploded: false
    property real probablity: 0
    property point cell: Qt.point(0,0)
    property MobileCellCache cache

    width: cellCache.cellSize; height: cellCache.cellSize
    
    Rectangle {
        id: background
        anchors.fill: parent
        color: "#f0f0f0"
        radius: 4
        opacity: 0.8;
    }

    ProbablityItem {
        id: mine

        opacity: 1

        probablity: cell.probablity
        cache: cell.cache
        anchors.fill: parent
        anchors.margins: 3
        visible: !cell.opened && probablity >= 0
        opened: cell.opened
        safe: cell.safe

        Behavior on probablity {NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }
    }

    TrivialShader {
        id: trivialShader
        anchors.fill: parent
        visible: opened
        source: cache.bombTextSources[cell.nearBombCount]
    }



    Image {
        anchors.fill: parent
        visible: !opened
        source: "glass.png"
    }

    Image {
        id: flag
        width: cell.width - 6; height: cell.height - 6
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -cell.height*2
        visible: false
        opacity: 0
        source: "flag.png"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            field.click(cell.cell);
        }

        onDoubleClicked: {
            field.douleClick(cell.cell);
        }
    }

    states: [
        State {
            name: "flagged"
            when: flagged
            PropertyChanges {
                target: flag
                visible: true
                anchors.verticalCenterOffset: 0
                opacity: 1
            }
            PropertyChanges {
                target: mine
                visible: false
                opacity: 0
            }
        }, State {
            name: "exploded"
            when: exploded

            PropertyChanges {
                target: mine
                visible: true
                opacity: 1
                probablity: 1
            }
            PropertyChanges {
                target: background
                color: "#FF0000"
            }
        }

    ]

    transitions: [
        Transition {
            to: "flagged"
            SequentialAnimation {
                PropertyAction { targets: [flag]; property: "visible" }
                ParallelAnimation {
                    NumberAnimation { target: mine; property: "opacity"; easing.type: Easing.OutQuad; duration: 1000 }

                    NumberAnimation {
                        target: flag;
                        property: "opacity";
                        easing.type: Easing.InQuad
                    }

                    NumberAnimation {
                        target: flag;
                        property: "anchors.verticalCenterOffset";
                        easing.type: Easing.OutBounce
                        duration: 700
                    }
                }
                PropertyAction { targets: [mine]; property: "visible" }
            }
        }
    ]
}
