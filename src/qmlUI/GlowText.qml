import QtQuick 2.4
import QtGraphicalEffects 1.0
import "."

Item {
    property alias text: text.text
    property alias color: text.color
    property alias font: text.font
    width: text.width
    height: text.width
    Glow {
        anchors.fill: text
        radius: font.pixelSize*15/20
        samples: font.pixelSize
        
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
