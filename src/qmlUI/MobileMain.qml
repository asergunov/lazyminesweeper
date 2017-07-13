import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtQml.Models 2.2

Item {
    id: main
    width: 600
    height: 800
    layer.enabled: true
    property QtObject model: QtObject {
        id: fakeData
        property var size: Qt.size(10,15);
        property var cells: ListModel {

            ListElement { row: 3; column: 0; probablity: 0.1 }
            ListElement { row: 3; column: 1; probablity: 0.2 }
            ListElement { row: 3; column: 2; probablity: 0.3 }
            ListElement { row: 3; column: 3; probablity: 0.4 }
            ListElement { row: 3; column: 4; probablity: 0.5 }
            ListElement { row: 3; column: 5; probablity: 0.6 }
            ListElement { row: 3; column: 6; probablity: 0.7 }
            ListElement { row: 3; column: 7; probablity: 0.8 }
            ListElement { row: 3; column: 8; probablity: 0.9 }
            ListElement { row: 3; column: 9; probablity: 1.0 }
            ListElement { row: 4; column: 0; bombNearCount: 0; opened: true }
            ListElement { row: 4; column: 1; bombNearCount: 1; opened: true }
            ListElement { row: 4; column: 2; bombNearCount: 2; opened: true }
            ListElement { row: 4; column: 3; bombNearCount: 3; opened: true }
            ListElement { row: 4; column: 4; bombNearCount: 4; opened: true }
            ListElement { row: 4; column: 5; bombNearCount: 5; opened: true }
            ListElement { row: 4; column: 6; bombNearCount: 6; opened: true }
            ListElement { row: 4; column: 7; bombNearCount: 7; opened: true }
            ListElement { row: 1; column: 1; safe: true }
            ListElement { row: 1; column: 3; bombNearCount: 1; opened: true }
            ListElement { row: 2; column: 1; bombNearCount: 2; opened: true }
            ListElement { row: 1; column: 2; bombNearCount: 3; flagged: true }
            ListElement { row: 1; column: 0; bombNearCount: 4; flagged: true }
        }
    }

    ParalaxFlickable {
        id: fieldContainer

        image: Image {
            fillMode: Image.PreserveAspectCrop
            source: "ginkaku-ji-temple-kyoto-japan-161247.jpeg"
        }

        anchors.fill: parent
        anchors.topMargin: topPanel.height

        contentWidth: field.width
        contentHeight: field.height

        MoblileField {
            id: field

            rows: model.size.width
            columns: model.size.height
            rowHeight: cellCache.cellSize; columnWidth: cellCache.cellSize;

            function click(cell) {
                model.click(cell);
            }

            function douleClick(cell) {
                model.douleClick(cell);
            }

            cellsModel: model.cells
            cellDelegate: MobileCell {
                id: cell
                width: field.columnWidth; height: field.rowHeight
                nearBombCount: model.nearBombCount ? model.nearBombCount : 0
                probablity: model.probablity
                flaged: model.flagged
                opened: model.opened
                safe: model.safe
                cell: model.position
                x: cell.cell.x * (width+field.spacing); y: cell.cell.y * (height+field.spacing)
                cache: cellCache
            }

        }
    }

    MobileCellCache {
        id:  cellCache
        cellSize: 45*fieldContainer.scale
    }

    MobileTopPanel {
        id: topPanel
        bombRemains: model.bombRemains
        onMakeBestTurn: model.makeBestTurn()
        onRestart: {
            //model.init(Qt.size(8,8),10);
            model.init(Qt.size(16,16),40);
            //model.init(Qt.size(25,25),99);
        }
    }
}
