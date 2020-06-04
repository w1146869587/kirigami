import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10 as QQC2
import org.kde.kirigami 2.13

QtObject {
    property SequentialAnimation animation: SequentialAnimation {
        id: __animator
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
        ScriptAction {
            script: __aniLoader.source = ""
        }
    }
}