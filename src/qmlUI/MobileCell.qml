import QtQuick 2.4
import QtQuick.Templates 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

Item {
    id:cell
    property int nearBombCount: 4
    
    width: 30
    height: 30
    
    Rectangle {
        anchors.fill: parent
        color: "white"
        radius: 4
        opacity: 0.8
    }
    
    Text {
        anchors.centerIn: parent
        text: nearBombCount
        font.bold: true
    }
}
