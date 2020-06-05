varying highp vec2 qt_TexCoord0;
uniform sampler2D source;
uniform sampler2D maskSource;
uniform lowp float qt_Opacity;
uniform lowp float progress;

void main() {
    gl_FragColor = texture2D(source, qt_TexCoord0) * texture2D(maskSource, qt_TexCoord0).a * qt_Opacity *  (1.0 - progress);
}