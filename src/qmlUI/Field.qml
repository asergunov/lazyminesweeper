import QtQuick 2.0

Rectangle {
    id: main;
    width: grid.columns*20+10;
    height: grid.rows*20+10;

    signal updateCells();
    function restart() {
        Field.init(Qt.size(24, 24), 99);
        updateField();
        grid.forceActiveFocus();
    }

    function updateField() {
        updateCells();
    }

    Component.onCompleted: restart();

    Grid {
        focus: true;
        id: grid;
        //spacing: 2;
        columns: Field.size.width;
        rows: Field.size.height;

        Repeater {
            model: grid.columns * grid.rows;
            delegate: Item {
                focus: true;
                id: cell;
                property variant modelIndex: Qt.point(index%grid.columns, Math.floor(index/grid.columns));
                property int value;
                property bool flag;
                width: 20; height: 20;

                Component.onCompleted: updateCell();

                function updateCell() {
                    value = Field.bombsNearCount(modelIndex);
                    if(value >= 0) {
                        console.debug("value:"+value+" at "+modelIndex);
                        background.color = "transparent";
                    }

                    var p = Field.minePorabablity(modelIndex);
                    if(p < 0) {
                        background.color = "transparent";
                    } else if(p==0) {
                        background.color = "#7F00FF00";
                    } else if(p==1) {
                        background.color = "red";
                    } else {
                        background.color = Qt.rgba(0,0,0, p);
                    }
                    //borderCheck.visible = (index in Field.border);
                    //numberCheck.visible = (index in Field.numbers);
                }

                Connections {
                    target: main;
                    onUpdateCells: cell.updateCell();
                }

                Rectangle {
                    id: background
                    anchors.fill: parent;
                    Behavior on color { ColorAnimation { duration: 200 } }
                    anchors.margins: 1;
                }

                Text {
                    anchors.centerIn: parent;
                    text: cell.value > 0?cell.value:"";
                    visible: cell.value > 0;
                }

                Rectangle {
                    anchors.fill: parent;
                    border.color: "gray";
                    visible: cell.activeFocus;
                }

                Keys.onSpacePressed: {
                    Field.click(modelIndex);
                    main.updateField();
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        Field.click(modelIndex);
                        main.updateField();
                    }
                    hoverEnabled: true;
                    onEntered: console.log(Field.minePorabablity(modelIndex));
                }
            }
        }
    }

}
