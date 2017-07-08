import QtQuick 2.4

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
    
    Grid {
        id: grid
        anchors.centerIn: parent
        spacing: field.spacing
        Repeater {
            id: repeater
            delegate: cellDelegate
        }
        
    }
    
}
