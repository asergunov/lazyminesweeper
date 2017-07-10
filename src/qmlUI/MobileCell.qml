import QtQuick 2.4
import QtGraphicalEffects 1.0

Item {
    id:cell
    property int nearBombCount: 4
    property bool flaged : false
    property bool safe : false
    property bool opened : false
    property real probablity: -1
    property point cell: Qt.point(0,0)

    // Rectangle { color: "#ff0000"; anchors.fill: parent; opacity: 0.3; }

    property variant textColors: {
        return [
        "#00000000",
        "#0100fe", // 1
        "#017f01", // 2
        "#fe0000", // 3
        "#010080", // 4
        "#810102", // 5
        "#008081", // 6
        "#000000",
        "#808080"
        ];
    }

    width: 30
    height: 30
    
    Rectangle {
        id: background
        anchors.fill: parent
        color: "#f0f0f0"
        radius: 4
        opacity: 0.8;
    }


//    Image {
//        anchors.fill: parent
//        source: "green.png"
//        opacity: (safe && !opened) ? 0.5 : 0
//        Behavior on opacity {
//            NumberAnimation {
//                easing.type: Easing.InOutQuad
//            }
//        }
//    }

    Item {
        id: probablityItem
        anchors.fill: parent
        visible: !opened && probablity >= 0
        anchors.margins: 3

        Image {
            opacity: 0.15
            id: mine
            anchors.fill: parent
            source: "mine.png"
        }

        Canvas {
            id: arc
            anchors.fill: parent
            visible: false
            property real probablity: cell.probablity
            onProbablityChanged: arc.requestPaint()
            Behavior on probablity {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                }
            }
            onPaint: {
                var ctx = getContext("2d");
                ctx.reset();

                var centreX = width / 2;
                var centreY = height / 2;

                ctx.beginPath();
                ctx.fillStyle = "black";
                ctx.moveTo(centreX, centreY);
                ctx.arc(centreX, centreY, width / 2, Math.PI, Math.PI + Math.PI * 2 * probablity, false);
                ctx.lineTo(centreX, centreY);
                ctx.fill();
            }
        }

        OpacityMask {
            anchors.fill: parent
            source: mine
            maskSource: arc
            opacity: 0.5
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

//        Rectangle {
//            id: probablityBackground
//            anchors.fill: parent
//            //color: Qt.darker(Qt.rgba(1,1,1,1), 1.0+probablity)
//            color: Qt.hsla(0, 0, 1-probablity, 1)
//            radius: 4
//            opacity: 0.8;
//        }
    }

    Item {
        anchors.fill: parent
        visible: opened
        Glow {
              anchors.fill: bombsText
              radius: 15
              samples: 20

              color: "white"
              source: bombsText
          }

        Text {
            id: bombsText
            anchors.centerIn: parent
            text: nearBombCount
            font.bold: true
            font.pixelSize: 20
            color: textColors[nearBombCount]
        }
    }



    Image {
        id: flag
        width: cell.width - 6; height: cell.height - 6
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -cell.height*2
        //anchors.margins: 3
        visible: false
        opacity: 0
        source: "flag.png"
    }

    Image {
        anchors.fill: parent
        visible: !opened
        source: "glass.png"
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
                    NumberAnimation { target: mine; property: "opacity"; easing.type: Easing.OutQuad; duration: 100 }

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
