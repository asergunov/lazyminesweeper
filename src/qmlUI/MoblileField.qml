import QtQuick 2.4

/*!
  \qmltype MoblileField
  \brief Fileld for mobile board
  */
Item {
    id: field
    
    width: Math.max(grid.width, fieldContainer.width)
    height: Math.max(grid.height, fieldContainer.height)

    property int rows: grid.rows
    property int columns: grid.columns
    property int rowHeight: 30
    property int columnWidth: 30
    property real spacing : 2;

    property Component cellDelegate : Text {
        text: "none"
    }
    property alias cellsModel: repeater.model
    
    Item {
        id: grid
        anchors.centerIn: parent
        //spacing: field.spacing
        width: columns * columnWidth + spacing * (columns-1);
        height: rows * rowHeight + spacing * (rows-1);
        Repeater {
            id: repeater
            delegate: cellDelegate
        }
        
    }
    
}
