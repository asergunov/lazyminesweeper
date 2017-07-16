import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtQml.Models 2.2

Item {
    id: main
    width: 600
    height: 800
    layer.enabled: true
    property QtObject model: DummyModel {}

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
        cache: cellCache
        bombRemains: model.bombRemains
        onMakeBestTurn: model.makeBestTurn()
        onRestart: {
            //model.init(Qt.size(8,8),10);
            model.init(Qt.size(16,16),40);
            //model.init(Qt.size(25,25),99);
        }
        solverActive: true
        solverInprogress: model.solverRunning
    }
}
