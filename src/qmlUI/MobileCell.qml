import QtQuick 2.4
import QtGraphicalEffects 1.0

Item {
    id:cell
    property int nearBombCount: 4
    property bool flaged : false
    property bool safe : false
    property bool opened : false
    property real probablity: 0.5

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
    
    Item {
        anchors.fill: parent
        visible: !flaged && !safe && !opened
        Rectangle {
            anchors.fill: parent
            //color: Qt.darker(Qt.rgba(1,1,1,1), 1.0+probablity)
            color: Qt.hsla(0, 0, 1-probablity, 1)
            radius: 4
            opacity: 0.8;
        }
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
        anchors.fill: parent
        visible: flaged
        source: "red.svg"
    }

    Image {
        anchors.fill: parent
        visible: safe
        source: "green.svg"
    }

    Image {
        anchors.fill: parent
        visible: !opened
        source: "glass.svg"
    }
}
