import QtQuick 2.0

Flickable {
    id: flickable

    property Component image : Item {}
    property alias background: background

    property real pralaxStrengh: 0.1
    property real overshotMargin: 1

    Item {
        id: background
        anchors.fill: parent
        parent: flickable
        z: -1
        Loader {
            sourceComponent: image
            width: flickable.width*(1+2*overshotMargin*pralaxStrengh) + (pralaxStrengh)*(flickable.contentItem.width-flickable.width)
            height: flickable.height*(1+2*overshotMargin*pralaxStrengh) + (pralaxStrengh)*(flickable.contentItem.height-flickable.height)

            x: -flickable.width*overshotMargin*pralaxStrengh - flickable.contentX*pralaxStrengh
            y: -flickable.height*overshotMargin*pralaxStrengh - flickable.contentY*pralaxStrengh
        }
    }
}
