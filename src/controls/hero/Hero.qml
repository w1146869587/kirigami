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
        Loader {
            id: __aniLoader
            onStatusChanged: if (__aniLoader.status == Loader.Ready) item.animation.restart()
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
        __private.toTransformX = (target.ScenePosition.x - ScenePosition.x)
        __private.toTransformY = (target.ScenePosition.y - ScenePosition.y)
        __private.toTransformWidth = target.width+0
        __private.toTransformHeight = target.height+0
        __private.returnWidth = child.width+0
        __private.returnHeight = child.height+0
        __aniLoader.source = "Animation.qml"
    }
}