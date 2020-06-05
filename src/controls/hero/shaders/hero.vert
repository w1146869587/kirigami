uniform highp mat4 qt_Matrix;
attribute highp vec4 qt_Vertex;
attribute highp vec2 qt_MultiTexCoord0;
varying highp vec2 qt_TexCoord0;
uniform highp float startX;
uniform highp float startY;
uniform highp float targetX;
uniform highp float targetY;
uniform highp float scaleWidth;
uniform highp float scaleHeight;
uniform highp float progress;

highp mat4 morph = mat4(1.0 + (scaleWidth - 1.0) * progress, 0.0, 0.0, startX*(1.0-progress) + targetX*progress,
                        0.0, 1.0 + (scaleHeight - 1.0) * progress, 0.0, startY*(1.0-progress) + targetY*progress,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0);

void main() {
    qt_TexCoord0 = qt_MultiTexCoord0;
    gl_Position = qt_Matrix * qt_Vertex * morph;
}
