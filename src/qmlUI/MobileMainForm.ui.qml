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

            property real pralaxStrengh: 0.3
            width: fieldContainer.contentWidth*(1+pralaxStrengh)
            height: fieldContainer.contentHeight*(1+pralaxStrengh)

            x: -fieldContainer.contentX*pralaxStrengh
            y: -fieldContainer.contentY*pralaxStrengh
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

    Flickable {
        id: fieldContainer
        anchors.fill: parent

        contentWidth: field.width
        contentHeight: field.height

        Item {
            id: field

            width: 1024
            height: 768

            DropArea {
                x: 75; y: 75
                width: 50; height: 50

                Rectangle {
                    anchors.fill: parent
                    color: "green"

                    visible: parent.containsDrag
                }
            }

            ColorOverlay {
                id: bgDarkner2
                anchors.fill: parent
                source: ShaderEffectSource {
                    sourceItem: background
                    sourceRect: field.mapToItem(background, cell.x, cell.y, cell.width, cell.height)
                }
                color: "#80ffffff"
                visible: false
            }


            FastBlur {
                id: cell
                width: 30
                height: 30
                x: 100
                y: 100
                //anchors.centerIn: parent

                radius: 32
                source: bgDarkner2

                MouseArea {
                    id: dragArea
                    anchors.fill: parent
                    drag.target: parent
                }
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
