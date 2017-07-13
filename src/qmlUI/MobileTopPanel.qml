import QtQuick 2.4
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0

Item {
    id: topPanel
    height: 40
    property int bombRemains: 99
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
    
    GlowText {
        id: bombRemains
        width: 30; height: 30
        anchors.centerIn: parent
        text: topPanel.bombRemains
    }

    Row {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 4

        Button {
            id: restartButton
            text: "Restart"
            onClicked: restart();
        }
    }
    
}
