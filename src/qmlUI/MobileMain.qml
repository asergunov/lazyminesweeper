import QtQuick 2.4
import QtQuick.Templates 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

Item {
    width: 600
    height: 800

    layer.enabled: true

    Item {
        id: background
        anchors.fill: parent
        Image {
            fillMode: Image.PreserveAspectCrop
            source: "ginkaku-ji-temple-kyoto-japan-161247.jpeg"

            //anchors.fill: parent

            property real pralaxStrengh: 0.1
            property real overshotMargin: 1
            width: fieldContainer.width*(1+2*overshotMargin*pralaxStrengh) + (pralaxStrengh)*(fieldContainer.contentItem.width-fieldContainer.width)
            height: fieldContainer.height*(1+2*overshotMargin*pralaxStrengh) + (pralaxStrengh)*(fieldContainer.contentItem.height-fieldContainer.height)

            x: -fieldContainer.width*overshotMargin*pralaxStrengh - fieldContainer.contentX*pralaxStrengh
            y: -fieldContainer.height*overshotMargin*pralaxStrengh - fieldContainer.contentY*pralaxStrengh
        }
    }

    Flickable {
        id: fieldContainer
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
                spacing: 5

                Repeater {
                    model: grid.rows*grid.columns
                    delegate: Item {
                        id:cell

                        width: 30
                        height: 30

                        ColorOverlay {
                            id: bgDarkner
                            anchors.fill: parent
                            source: ShaderEffectSource {
                                function update() {
                                    sceduleUpdate();
                                }
                                live: false
                                sourceItem: background
                                sourceRect: mapToItem(background, 0, 0, cell.width, cell.height)
                            }
                            color: "#80ffffff"
                        }


                        FastBlur {
                            anchors.centerIn: parent
                            radius: 32
                            source: bgDarkner
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: "red"
                            opacity: 0.1
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
                sourceItem: background
                sourceRect: mapToItem(background, 0, 0, topPanel.width, topPanel.height)
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
