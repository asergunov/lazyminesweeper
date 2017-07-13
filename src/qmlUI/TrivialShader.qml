import QtQuick 2.4
import QtGraphicalEffects 1.0
import "."

ShaderEffect {
    id: trivialShader
    property variant source
    
    vertexShader: "
                        uniform highp mat4 qt_Matrix;
                        attribute highp vec4 qt_Vertex;
                        attribute highp vec2 qt_MultiTexCoord0;
                        varying highp vec2 uv;
                        void main() {
                            uv = qt_MultiTexCoord0;
                            gl_Position = qt_Matrix * qt_Vertex;
                        }"
    fragmentShader: "
                        varying highp vec2 uv;
                        uniform sampler2D source;
                        uniform lowp float qt_Opacity;
                        void main() {
                            lowp vec4 tex = texture2D(source, uv);
                            gl_FragColor = tex * qt_Opacity;
                        }"
}
