import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtQml.Models 2.2

QtObject {
    id: fakeData
    property size size: Qt.size(10,15);
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
