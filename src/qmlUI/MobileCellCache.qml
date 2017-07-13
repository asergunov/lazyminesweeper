import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: cache
    property int arcCount: 64
    property alias probabilitySources: porabablityListItem.children
    property alias bombTextSources: bombTextSourcesItem.children
    property real cellSize: 40
    property real probabilitySourceSize: cellSize-6
    visible: false;

    function probabilitySource(t) {
        var index = Math.floor(Math.min(1, Math.max(0, t))*arcCount);
        if(index >=0 && index < probabilitySources.length)
            return probabilitySources[index];
    }

    Item {
        id: porabablityListItem
        Repeater {
            id: porabablityListItemRepeater
            model: arcCount+1
            ShaderEffectSource {
                sourceItem: Item {
                    width: probabilitySourceSize; height: probabilitySourceSize
                    Image {
                        opacity: 0.15
                        id: mine
                        anchors.fill: parent
                        source: "mine.svg"
                    }

                    OpacityMask {
                        anchors.fill: parent
                        source: mine
                        maskSource: ShaderEffectSource {
                            property alias probability: canvas.probablity
                            mipmap: true
                            sourceItem: ArcCanvas {
                                id: canvas
                                width: 128; height: 128;
                                probablity: index/arcCount
                            }
                        }
                        opacity: 0.5
                    }
                }
            }
        }
    }

    Item {
        id: bombTextSourcesItem
        Repeater {
            id: bombTextSourcesItemRepeater
            model: [
                "#00000000",
                "#0100fe", // 1
                "#017f01", // 2
                "#fe0000", // 3
                "#010080", // 4
                "#810102", // 5
                "#008081", // 6
                "#000000",
                "#808080"
                ];
            ShaderEffectSource {
                sourceItem: GlowText {
                    id: text
                    width: cellSize
                    height: cellSize
                    font.pixelSize: cellSize*0.5
                    text: index
                    color: modelData
                }
            }
        }
    }
}
