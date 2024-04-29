import QtQuick 2.7
import QtQuick.Controls 2.0

import minesweeper 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Zen Mines")

//    MobileMain {
//        model: Field {
//        }
//        anchors.fill: parent
//    }

//  QTBUG-61434 workaround
    Component {
        id: main
        MobileMain {
            model: Field {
            }
            anchors.fill: parent
        }
    }

    Component.onCompleted: {
        main.createObject(contentItem);
    }
}
