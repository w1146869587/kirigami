/*
 *  SPDX-FileCopyrightText: 2018 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.14
import "../" as Private


Controls.ToolBar {
    id: root

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
        bottomMargin: root.page.footer ? root.page.footer.height : 0
    }

    clip: true

    readonly property Page page: root.parent.page
    //either Action or QAction should work here

    function isActionAvailable(action) { return action && (action.hasOwnProperty("visible") ? action.visible === undefined || action.visible : !action.hasOwnProperty("visible")); }

    readonly property QtObject action: root.page && isActionAvailable(root.page.mainAction) ? root.page.mainAction : null
    readonly property QtObject leftAction: root.page && isActionAvailable(root.page.leftAction) ? root.page.leftAction : null
    readonly property QtObject rightAction: root.page && isActionAvailable(root.page.rightAction) ? root.page.rightAction : null

    readonly property bool hasApplicationWindow: typeof applicationWindow !== "undefined" && applicationWindow
    readonly property bool hasGlobalDrawer: typeof globalDrawer !== "undefined" && globalDrawer
    readonly property bool hasContextDrawer: typeof contextDrawer !== "undefined" && contextDrawer

    transform: Translate {
        id: translateTransform
        y: layout.internalVisibility ? 0 : root.height
        Behavior on y {
            NumberAnimation {
                duration: Units.longDuration
                easing.type: layout.internalVisibility == true ? Easing.InQuad : Easing.OutQuad
            }
        }
    }

    position: Controls.ToolBar.Footer

    MouseArea {
        anchors.fill: parent
        onClicked: page.forceActiveFocus()
    }

   /* ScenePosition.onXChanged: {
        if (ScenePosition.x === 0) {
            applicationWindow().pageStack.globalToolBar.leftHandleAnchor = leftHandleAnchor
        }
    }*/
    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.rightMargin: Units.smallSpacing
        spacing: Units.smallSpacing
        
        property bool internalVisibility: (!root.hasApplicationWindow || (applicationWindow().controlsVisible && applicationWindow().height > root.height*2)) && (root.action === null || root.action.visible === undefined || root.action.visible)

        Item {
            id: leftHandleAnchor
            /*visible: (typeof applicationWindow() !== "undefined" && applicationWindow().globalDrawer && applicationWindow().globalDrawer.enabled && applicationWindow().globalDrawer.handleVisible &&
            (applicationWindow().globalDrawer.handle.handleAnchor == (Qt.application.layoutDirection == Qt.LeftToRight ? leftHandleAnchor : rightHandleAnchor)))*/


            Layout.fillHeight: true
            Layout.preferredWidth: height
        }
        Item {Layout.fillWidth: true}
        ActionToolBar {
            id: toolBar

            Layout.alignment: Qt.AlignVCenter
            Layout.maximumWidth: implicitWidth
            Layout.fillWidth: true
            Layout.fillHeight: true

            visible: actions.length > 0
            alignment: pageRow.globalToolBar.toolbarActionAlignment
            heightMode: ToolBarLayout.ConstrainIfLarger

            actions: {
                var result = []

                if (page) {
                    if (page.actions.main) {
                        page.actions.main.displayHint |= DisplayHint.KeepVisible
                        result.push(page.actions.main)
                    }
                    if (page.actions.left) {
                        page.actions.left.displayHint |= DisplayHint.KeepVisible
                        result.push(page.actions.left)
                    }
                    if (page.actions.right) {
                        page.actions.right.displayHint |= DisplayHint.KeepVisible
                        result.push(page.actions.right)
                    }
                    if (page.actions.contextualActions.length > 0) {
                        result = result.concat(Array.prototype.map.call(page.actions.contextualActions, function(item) { return item }))
                    }
                }

                return result
            }
        }
        Item {Layout.fillWidth: true}
    }
}

