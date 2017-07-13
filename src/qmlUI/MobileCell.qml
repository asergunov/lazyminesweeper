import QtQuick 2.4
import QtGraphicalEffects 1.0
import "."

Item {
    id:cell
    property int nearBombCount: 4
    property bool flaged : false
    property bool safe : false
    property bool opened : false
    property real probablity: -1
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

    Item {
        id: probablityItem
        anchors.fill: parent
        visible: !opened && probablity >= 0
        anchors.margins: 3
        property real probablity: cell.probablity

        Behavior on probablity {
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }

        TrivialShader {
            id: mine
            opacity: 1
            visible: true
            source: cache.probabilitySource(probablityItem.probablity);
            width: source.sourceItem.width; height: source.sourceItem.height
            anchors.centerIn: parent
        }

        Image {
            anchors.fill: parent
            source: "green.png"
            opacity: (safe && !opened) ? 0.2 : 0
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                }
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
