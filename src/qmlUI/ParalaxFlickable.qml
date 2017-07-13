import QtQuick 2.0

Flickable {
    id: flickable

    property Component image
    property alias background: background

    property real pralaxStrengh: 0.1
    property real overshotMargin: 1

//    property alias scale: pinchArea.scale
//    PinchArea {
//        id: pinchArea
//        width: Math.max(flickable.contentWidth, flickable.width)
//        height: Math.max(flickable.contentHeight, flickable.height)

//        property real initialWidth
//        property real initialHeight
//        property real scale: 1
//        onPinchStarted: {
//            initialWidth = flickable.contentWidth
//            initialHeight = flickable.contentHeight
//        }

//        onPinchUpdated: {
//            // adjust content pos due to drag
//            flickable.contentX += pinch.previousCenter.x - pinch.center.x
//            flickable.contentY += pinch.previousCenter.y - pinch.center.y

//            // resize content
//            scale = pinch.scale;
//            flickable.resizeContent(initialWidth * pinch.scale, initialHeight * pinch.scale, pinch.center)
//        }

//        onPinchFinished: {
//            // Move its content within bounds.
//            flickable.returnToBounds()
//        }

        Item {
            id: background
            //anchors.fill: parent
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
//    }


}
