import QtQuick 2.4
import QtGraphicalEffects 1.0
import "."

Item {
    property alias text: text.text
    property alias color: text.color
    property alias font: text.font
    Glow {
        anchors.fill: text
        radius: 15
        samples: 20
        
        color: "white"
        source: text
    }
    
    Text {
        id: text
        anchors.centerIn: parent
        font.bold: true
        font.pixelSize: 20
    }
}
