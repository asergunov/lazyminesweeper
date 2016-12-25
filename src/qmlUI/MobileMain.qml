import QtQuick 2.4
import QtQuick.Templates 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

Item {
    width: 600
    height: 800

    layer.enabled: true

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

            rows: 10
            columns: 10

            cellDelegate: MobileCell {
                id: cell
            }

        }
    }

    MobileTopPanel {
        id: topPanel
    }

    MoblieBottomPanel {
        id: bottomPanel
    }
}
