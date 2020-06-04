import QtQuick 2.10
import QtQuick.Layouts 1.10
import QtQuick.Controls 2.10 as QQC2
import org.kde.kirigami 2.13

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

                    __private.parent.width = __private.parent.width + 0
                    __private.parent.height = __private.parent.height + 0
                    __private.parent.implicitWidth = __private.parent.implicitWidth + 0
                    __private.parent.implicitHeight = __private.parent.implicitHeight + 0

                    __private.parent.Layout.alignment = (__private.parent.Layout.alignment)
                    __private.parent.Layout.bottomMargin = __private.parent.Layout.bottomMargin+0
                    __private.parent.Layout.column = __private.parent.Layout.column+0
                    __private.parent.Layout.columnSpan = __private.parent.Layout.columnSpan+0
                    __private.parent.Layout.fillHeight = __private.parent.Layout.fillHeight+0
                    __private.parent.Layout.fillWidth = __private.parent.Layout.fillWidth+0
                    __private.parent.Layout.leftMargin = __private.parent.Layout.leftMargin+0
                    __private.parent.Layout.margins = __private.parent.Layout.margins+0
                    __private.parent.Layout.maximumHeight = __private.parent.Layout.maximumHeight+0
                    __private.parent.Layout.maximumWidth = __private.parent.Layout.maximumWidth+0
                    __private.parent.Layout.minimumHeight = __private.parent.Layout.minimumHeight+0
                    __private.parent.Layout.minimumWidth = __private.parent.Layout.minimumWidth+0
                    __private.parent.Layout.preferredHeight = __private.parent.Layout.preferredHeight+0
                    __private.parent.Layout.preferredWidth = __private.parent.Layout.preferredWidth+0
                    __private.parent.Layout.rightMargin = __private.parent.Layout.rightMargin+0
                    __private.parent.Layout.row = __private.parent.Layout.row+0
                    __private.parent.Layout.rowSpan = __private.parent.Layout.rowSpan+0
                    __private.parent.Layout.topMargin = __private.parent.Layout.topMargin+0
                }
            }
            ParallelAnimation {
                NumberAnimation { target: __transformer; id: __transformerX; properties: "x"; duration: Units.longDuration-50; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __transformer; id: __transformerY; properties: "y"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __private.parent.child; id: __transformerWidth; properties: "implicitWidth,width"; duration: Units.longDuration-50; easing.type: Easing.InOutExpo }
                NumberAnimation { target: __private.parent.child; id: __transformerHeight; properties: "implicitHeight,height"; duration: Units.longDuration; easing.type: Easing.InOutExpo }
            }
            NumberAnimation { target: __private.buddy; properties: "opacity"; from: 0; to: 1 }
            NumberAnimation { target: __private.parent.child; properties: "opacity"; from: 1; to: 0 }
            ParallelAnimation {
                NumberAnimation { target: __transformer; id: __returnX; properties: "x"; }
                NumberAnimation { target: __transformer; id: __returnY; properties: "y"; }
                NumberAnimation { target: __private.parent.child; id: __returnWidth; properties: "implicitWidth,width"; }
                NumberAnimation { target: __private.parent.child; id: __returnHeight; properties: "implicitHeight,height";}
            }
            ScriptAction {
                script: {
                    __private.parent.width = Qt.binding(() => child.width)
                    __private.parent.height = Qt.binding(() => child.height)
                    __private.parent.implicitWidth = Qt.binding(() => child.implicitWidth)
                    __private.parent.implicitHeight = Qt.binding(() => child.implicitHeight)

                    __private.parent.Layout.alignment = Qt.binding(() => child.Layout.alignment)
                    __private.parent.Layout.bottomMargin = Qt.binding(() => child.Layout.bottomMargin)
                    __private.parent.Layout.column = Qt.binding(() => child.Layout.column)
                    __private.parent.Layout.columnSpan = Qt.binding(() => child.Layout.columnSpan)
                    __private.parent.Layout.fillHeight = Qt.binding(() => child.Layout.fillHeight)
                    __private.parent.Layout.fillWidth = Qt.binding(() => child.Layout.fillWidth)
                    __private.parent.Layout.leftMargin = Qt.binding(() => child.Layout.leftMargin)
                    __private.parent.Layout.margins = Qt.binding(() => child.Layout.margins)
                    __private.parent.Layout.maximumHeight = Qt.binding(() => child.Layout.maximumHeight)
                    __private.parent.Layout.maximumWidth = Qt.binding(() => child.Layout.maximumWidth)
                    __private.parent.Layout.minimumHeight = Qt.binding(() => child.Layout.minimumHeight)
                    __private.parent.Layout.minimumWidth = Qt.binding(() => child.Layout.minimumWidth)
                    __private.parent.Layout.preferredHeight = Qt.binding(() => child.Layout.preferredHeight)
                    __private.parent.Layout.preferredWidth = Qt.binding(() => child.Layout.preferredWidth)
                    __private.parent.Layout.rightMargin = Qt.binding(() => child.Layout.rightMargin)
                    __private.parent.Layout.row = Qt.binding(() => child.Layout.row)
                    __private.parent.Layout.rowSpan = Qt.binding(() => child.Layout.rowSpan)
                    __private.parent.Layout.topMargin = Qt.binding(() => child.Layout.topMargin)
                }
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