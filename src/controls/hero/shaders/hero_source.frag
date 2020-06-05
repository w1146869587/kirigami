varying highp vec2 qt_TexCoord0;
uniform sampler2D source;
uniform lowp float qt_Opacity;
uniform lowp float progress;

void main() {
    gl_FragColor = texture2D(source, qt_TexCoord0) * qt_Opacity * (1.0 - progress);
}