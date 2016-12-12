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

        Item {
            id: field

            width: Math.max(grid.width, fieldContainer.width)
            height: Math.max(grid.height, fieldContainer.height)

            Grid {
                anchors.centerIn: parent
                id: grid

                rows: 30
                columns: 30
                spacing: 2

                Repeater {
                    model: grid.rows*grid.columns
                    delegate: Item {
                        id:cell
                        property int nearBombCount: 4

                        width: 30
                        height: 30

//                        ColorOverlay {
//                            id: bgDarkner
//                            anchors.fill: parent
//                            source: ShaderEffectSource {
//                                live: true
//                                sourceItem: fieldContainer.background
//                                sourceRect: mapToItem(fieldContainer.background, 0, 0, cell.width, cell.height)
//                            }
//                            color: "#80ffffff"
//                        }


//                        FastBlur {
//                            anchors.centerIn: parent
//                            radius: 32
//                            source: bgDarkner
//                        }

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

                }

            }

        }
    }

    Item {
        id: topPanel
        height: 40

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        ColorOverlay {
            id: bgDarkner
            anchors.fill: parent
            source: ShaderEffectSource {
                sourceItem: fieldContainer.background
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

        Frame {
            width: 30
            height: 30
            anchors.centerIn: parent
            id: bombsRemain
            Text {
                text: "99"
                anchors.centerIn: parent
            }
        }

    }

    RowLayout {
        anchors.bottom: parent.bottom
        Button {
            id: bestTurnButton
        }

        Button {
            id: restartButton
        }
    }
}
