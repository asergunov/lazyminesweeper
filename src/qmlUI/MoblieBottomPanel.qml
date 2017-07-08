import QtQuick 2.4
import QtQuick.Templates 2.0
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

RowLayout {
    id: bottomPanel

    signal makeBestTurn();
    signal restart();

    anchors.bottom: parent.bottom
    Button {
        id: bestTurnButton
        text: "Best turn"
        onClicked: makeBestTurn();
    }
    
    Button {
        id: restartButton
        text: "Restart"
        onClicked: restart();
    }
}
