import QtQuick 2.4
import QtGraphicalEffects 1.0

Canvas {
    id: arc
    property real probablity: 0.3
    onProbablityChanged: arc.requestPaint()
    onPaint: {
        var ctx = getContext("2d");
        ctx.reset();
        
        var centreX = width / 2;
        var centreY = height / 2;
        
        ctx.beginPath();
        ctx.fillStyle = "black";
        ctx.moveTo(centreX, centreY);
        ctx.arc(centreX, centreY, width / 2, Math.PI, Math.PI + Math.PI * 2 * probablity, false);
        ctx.lineTo(centreX, centreY);
        ctx.fill();
    }
}
