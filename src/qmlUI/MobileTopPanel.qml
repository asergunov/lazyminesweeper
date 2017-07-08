import QtQuick 2.4
import QtQuick.Templates 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

Item {
    id: topPanel
    height: 40
    property int bombRemains: 99

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
            sourceItem: fieldContainer.background
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
    
    Item {
        width: 30
        height: 30
        anchors.centerIn: parent
        id: bombRemains

        Glow {
            anchors.fill: bombsText
            radius: 15
            samples: 20

            color: "white"
            source: bombsText
        }

        Text {
            id: bombsText
            text: topPanel.bombRemains
            anchors.centerIn: parent
            font.bold: true
            font.pixelSize: 20
        }
    }
    
}
