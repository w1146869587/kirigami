import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10 as QQC2
import org.kde.kirigami 2.13
import "bindings.js" as Binder

Item {
    z: 100

    width: child.width
    height: child.height
    implicitWidth: child.implicitWidth
    implicitHeight: child.implicitHeight

    Layout.alignment: child.Layout.alignment
    Layout.bottomMargin: child.Layout.bottomMargin
    Layout.column: child.Layout.column
    Layout.columnSpan: child.Layout.columnSpan
    Layout.fillHeight: child.Layout.fillHeight
    Layout.fillWidth: child.Layout.fillWidth
    Layout.leftMargin: child.Layout.leftMargin
    Layout.margins: child.Layout.margins
    Layout.maximumHeight: child.Layout.maximumHeight
    Layout.maximumWidth: child.Layout.maximumWidth
    Layout.minimumHeight: child.Layout.minimumHeight
    Layout.minimumWidth: child.Layout.minimumWidth
    Layout.preferredHeight: child.Layout.preferredHeight
    Layout.preferredWidth: child.Layout.preferredWidth
    Layout.rightMargin: child.Layout.rightMargin
    Layout.row: child.Layout.row
    Layout.rowSpan: child.Layout.rowSpan
    Layout.topMargin: child.Layout.topMargin

    enum AnimationType {
        Resize,
        Transform
    }

    property int animationType: Hero.AnimationType.Resize

    // TODO: Wire up to page stuff for automatic hero animations between
    // heroes sharing the same tag in different routes
    property string tag

    Item {
        id: __private
        Translate {
            id: __transformer
        }
        property Item buddy: Item {property Item child: Item{}}
        property real toTransformX
        property real toTransformY
        property real toTransformWidth
        property real toTransformHeight
        property real returnWidth
        property real returnHeight
        readonly property Item uberRoot: {
            try {
                return applicationWindow().contentItem
            } catch (error) {
                let parent = __private;
                let previousParent = __private;
                while (parent != null) {
                    previousParent = parent;
                    parent = parent.parent;
                }
                return previousParent
            }
        }
        // TODO: Figure out why refactoring into own file causes a segfault
        SequentialAnimation {
            id: __resizeAni
            ScriptAction {
                script: {
                    __private.buddy.opacity = 0

                    __private.parent.child.parent = __private.uberRoot
                    __private.parent.child.x = __private.parent.ScenePosition.x
                    __private.parent.child.y = __private.parent.ScenePosition.y

                    Binder.breakBindings(__private.parent)
                }
            }
            ParallelAnimation {
                NumberAnimation { target: __transformer; id: __transformerX; to: __private.toTransformX; properties: "x"; duration: Units.longDuration-50; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __transformer; id: __transformerY; to: __private.toTransformY; properties: "y"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __private.parent.child; id: __transformerWidth; to: __private.toTransformWidth; properties: "width"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __private.parent.child; id: __transformerHeight; to: __private.toTransformHeight; properties: "height"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
            }
            NumberAnimation { target: __private.buddy; properties: "opacity"; from: 0; to: 1 }
            NumberAnimation { target: __private.parent.child; properties: "opacity"; from: 1; to: 0 }
            ParallelAnimation {
                NumberAnimation { target: __transformer; id: __returnX; to: 0; properties: "x"; }
                NumberAnimation { target: __transformer; id: __returnY; to: 0; properties: "y"; }
                NumberAnimation { target: __private.parent.child; id: __returnWidth; to: __private.returnWidth; properties: "width"; }
                NumberAnimation { target: __private.parent.child; id: __returnHeight; to: __private.returnHeight; properties: "height";}
            }
            ScriptAction {
                script: Binder.restoreBindings(__private.parent, __private.parent.child)
            }
            ScriptAction {
                script: {
                    __private.parent.child.parent = __private.parent
                    __private.parent.child.x = 0
                    __private.parent.child.y = 0
                }
            }
            NumberAnimation { target: __private.parent.child; properties: "opacity"; from: 0; to: 1 }
        }
        // TODO: See above
        SequentialAnimation {
            id: openAnim
            ScriptAction {
                script: {
                    __private.parent.child.layer.enabled = true;
                    __private.parent.child.layer.smooth = true;
                    __private.buddy.child.layer.enabled = true;
                    __private.buddy.child.layer.smooth = true;
                    sourceEffect.visible = true;
                    destinationEffect.visible = true;
                    sourceEffect.source = null
                    sourceEffect.source = __private.parent.child
                    destinationEffect.source = null
                    destinationEffect.source = __private.buddy.child
                    __private.parent.child.opacity = 0
                    __private.buddy.child.opacity = 0
                    sourceEffect.parent.visible = true;
                }
            }
            ParallelAnimation {
                NumberAnimation {
                    target: sourceEffect
                    property: "progress"
                    from: 0
                    to: 1
                    duration: Units.longDuration
                    easing.type: Easing.InOutQuad
                }
                NumberAnimation {
                    target: destinationEffect
                    property: "progress"
                    from: 1
                    to: 0
                    duration: Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }
            ScriptAction {
                script: {
                    sourceEffect.visible = false;
                    destinationEffect.visible = false;
                    __private.parent.child.layer.enabled = false;
                    __private.parent.child.layer.smooth = false;
                    __private.buddy.child.layer.enabled = false;
                    __private.buddy.child.layer.smooth = false;
                    __private.buddy.child.opacity = 1
                    sourceEffect.parent.visible = false;
                }
            }

        }
        ShaderEffect {
            id: sourceEffect

            visible: false
            x: 0
            y: 0

            parent: __private.uberRoot
            width: __private.parent.width
            height: __private.parent.height

            property variant source: __private.parent.child
            property real progress: 0

            property real startX: __private.parent.ScenePosition.x / (__private.uberRoot.width / 2)
            property real startY: -__private.parent.ScenePosition.y / (__private.uberRoot.height / 2)

            property real targetX: scaleWidth - 1 + (__private.buddy.child.ScenePosition.x * 2) / __private.uberRoot.width
            property real targetY: 1-scaleHeight - (__private.buddy.child.ScenePosition.y * 2) / __private.uberRoot.height

            property real scaleWidth: __private.parent.width/__private.buddy.width
            property real scaleHeight: __private.parent.height/__private.buddy.height

            vertexShader: "shaders/hero.vert"
            fragmentShader: "shaders/hero_source.frag"
        }

        ShaderEffect {
            id: destinationEffect

            visible: false
            x: 0
            y: 0

            parent: __private.buddy.QQC2.Overlay.overlay
            width: __private.buddy.width
            height: __private.buddy.height

            property variant source: __private.buddy
            property real progress: sourceEffect.progress

            property real startX: __private.buddy.ScenePosition.x / (__private.uberRoot.width / 2)
            property real startY: -__private.buddy.ScenePosition.y / (__private.uberRoot.height / 2)

            property real targetX: scaleWidth - 1 + (__private.parent.child.ScenePosition.x * 2) / __private.uberRoot.width
            property real targetY: 1-scaleHeight - (__private.parent.child.ScenePosition.y * 2) / __private.uberRoot.height

            property real scaleWidth: __private.parent.child.width/__private.buddy.width
            property real scaleHeight: __private.parent.child.height/__private.buddy.height

            property variant maskSource: mask
            Rectangle {
                id: mask
                visible: false
                color: "white"
                radius: (width/2) * destinationEffect.progress
                width: (sourceEffect.width * sourceEffect.progress) +  (destinationEffect.width * destinationEffect.progress)
                height: (sourceEffect.height * sourceEffect.progress) +  (destinationEffect.height * destinationEffect.progress)
                layer.enabled: true
                layer.smooth: true
            }
            vertexShader: "shaders/hero.vert"
            fragmentShader: "shaders/hero_dest.frag"
        }
    }

    property Item background
    onBackgroundChanged: {
        background.parent = __private.parent
        background.anchors.fill = __private.parent
        background.z = 101
    }
    default property Item child
    onChildChanged: {
        child.parent = __private.parent
        child.transformOrigin = Item.TopLeft
        child.z = 102
    }

    function animateToHero(target) {
        console.assert(target instanceof Hero)
        __private.buddy = target
        if (__private.parent.animationType == Hero.AnimationType.Resize) {
            child.transform = [ __transformer ]
            __private.toTransformX = (target.ScenePosition.x - ScenePosition.x)
            __private.toTransformY = (target.ScenePosition.y - ScenePosition.y)
            __private.toTransformWidth = target.width+0
            __private.toTransformHeight = target.height+0
            __private.returnWidth = child.width+0
            __private.returnHeight = child.height+0
            __resizeAni.restart()
        } else {
            openAnim.restart()
        }
    }
}