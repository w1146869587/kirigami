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

    Item {
        id: __private
        Translate {
            id: __transformer
        }
        property Item buddy
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
        SequentialAnimation {
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
                NumberAnimation { target: __transformer; id: __transformerX; properties: "x"; duration: Units.longDuration-50; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __transformer; id: __transformerY; properties: "y"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __private.parent.child; id: __transformerWidth; properties: "width"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __private.parent.child; id: __transformerHeight; properties: "height"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
            }
            NumberAnimation { target: __private.buddy; properties: "opacity"; from: 0; to: 1 }
            NumberAnimation { target: __private.parent.child; properties: "opacity"; from: 1; to: 0 }
            ParallelAnimation {
                NumberAnimation { target: __transformer; id: __returnX; properties: "x"; }
                NumberAnimation { target: __transformer; id: __returnY; properties: "y"; }
                NumberAnimation { target: __private.parent.child; id: __returnWidth; properties: "width"; }
                NumberAnimation { target: __private.parent.child; id: __returnHeight; properties: "height";}
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
        child.transform = [ __transformer ]
        __transformerX.to = (target.ScenePosition.x - ScenePosition.x)
        __transformerY.to = (target.ScenePosition.y - ScenePosition.y)
        __transformerWidth.to = target.width+0
        __transformerHeight.to = target.height+0
        __returnX.to = 0
        __returnY.to = 0
        __returnWidth.to = child.width+0
        __returnHeight.to = child.height+0
        __animator.restart()
    }
}