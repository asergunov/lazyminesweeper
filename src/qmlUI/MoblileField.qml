import QtQuick 2.4
import QtQuick.Templates 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

/*!
  \qmltype MoblileField
  \brief Fileld for mobile board
  */
Item {
    id: field
    
    width: Math.max(grid.width, fieldContainer.width)
    height: Math.max(grid.height, fieldContainer.height)

    property alias rows: grid.rows
    property alias columns: grid.columns
    property real spacing : 2;

    property Component cellDelegate : Text {
        text: "none"
    }
    property alias cellsModel: repeater.model
    
    GridLayout {
        id: grid
        anchors.centerIn: parent
        columnSpacing: spacing
        rowSpacing: spacing
        Repeater {
            id: repeater
            delegate: cellDelegate
        }
        
    }
    
}
