import QtQuick 2.4
import QtQuick.Templates 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0
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

            function click(cell) {
                model.click(cell);
            }

            function douleClick(cell) {
                model.douleClick(cell);
            }

            cellsModel: model.cells
            cellDelegate: MobileCell {
                id: cell

                Layout.row: model.row
                Layout.column: model.column

                nearBombCount: model.nearBombCount ? model.nearBombCount : 0
                probablity: model.probablity
                flaged: model.flagged
                opened: model.opened
                safe: model.safe
                cell: model.position
            }

        }
    }

    MobileTopPanel {
        id: topPanel
    }

    MoblieBottomPanel {
        id: bottomPanel
        onMakeBestTurn: model.makeBestTurn()
        onRestart: model.init(Qt.size(25,25),99);
    }
}
