pragma Singleton

import QtQuick 2.0

Item {
    id: cache
    property int arcCount: 64
    property variant arcSources: []
    function arcSource(t) {
        return arcSources[Math.floor(Math.min(1, Math.max(0, t))*arcCount)];
    }

    Component {
        id: arcComponent
        ShaderEffectSource {
            property alias probability: canvas.probablity
            sourceItem: ArcCanvas {
                id: canvas
                width: 64; height: 64;
            }
        }
    }

    Component.onCompleted: {
        var sources = []
        for(var i = 0; i<=arcCount; ++i) {
            var arc = arcComponent.createObject(cache, {
                "probability":i/arcCount,
                //"live":false,
                //"recursive":false
            });
            arc.scheduleUpdate();
            sources.push(arc)
        }
        arcSources = sources
    }
}
