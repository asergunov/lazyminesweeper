import QtQuick 2.4
import QtQml.Models 2.2

Item {
    id: main
    width: 600
    height: 800
    layer.enabled: true
    property QtObject model: DummyModel {}

    function restart() {
        newGameDialog.shown = true;
        statsScreen.shown = false
    }

    Connections {
        target: model
        onGameOver: {
            // var win - true if win
            statsScreen.win = win;
            statsScreen.shown = true;
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
        cache: cellCache
        bombRemains: model.bombRemains
        onMakeBestTurn: model.makeBestTurn()
        onRestart: main.restart()
        solverActive: true
        solverInprogress: model.solverRunning
    }

    StatsScreen {
        id: statsScreen
        source: main
        visible: false
        parent: main.parent
        onRestart: main.restart()
    }

    NewGameDialog {
        id: newGameDialog
        source: main
        visible: false
        parent: main.parent
        onRestart: {
            model.init(size, bombCount);
            shown = false;
        }
    }
}
