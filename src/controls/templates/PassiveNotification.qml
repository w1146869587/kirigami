/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.3 as Controls
import QtQuick.Templates 2.5 as T2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.12 as Kirigami

/**
 * PassiveNotification is a type for small, passive and inline
notifications in the app.
 * used to show messages of limited importance that make sense only when
 * the user is using the application and wouldn't be suited as a global 
 * system-wide notification.
 * This is not a full-fledged notification system. the applciation should 
 * use this with care and only one notification should be visible at once per app.
*/
T2.Drawer {
    id: root

    /**
     * text: string
     * The text to display in the notification
     */
    property alias text: label.text

    /**
     * actionText: string
     * if present, the notification will have a button with this text
     */
    property alias actionText: actionButton.text

    /**
     * callBack: function
     * A JavaScript function that will be called when the user presses the action button
     */
    property var callBack

    x: parent.width/2 - width/2
    implicitWidth: Math.max(background ? background.implicitWidth : 0,
                            contentWidth + leftPadding + rightPadding) + leftInset + rightInset
    implicitHeight: Math.max(background ? background.implicitHeight : 0 ,
                             contentHeight + topPadding + bottomPadding)+ topInset + bottomInset

    topPadding: Kirigami.Units.smallSpacing
    leftPadding: Kirigami.Units.smallSpacing
    rightPadding: Kirigami.Units.smallSpacing
    bottomPadding: Kirigami.Units.smallSpacing

    //TODO: top-right edge on desktop?
    edge: Qt.BottomEdge
    modal: false
    closePolicy: T2.Popup.NoAutoClose
    topInset: Kirigami.Units.smallSpacing
    bottomInset: Kirigami.Units.smallSpacing
    focus: false

    function showNotification(message, timeout, actionText, callBack) {
        if (!message) {
            return;
        }
        text = message;
        root.actionText = actionText;
        root.callBack = callBack;

        if (timeout == "short") {
            timer.interval = 4000;
        } else if (timeout == "long") {
            timer.interval = 12000;
        } else if (timeout > 0) {
            timer.interval = timeout;
        } else {
            timer.interval = 7000;
        }

        open();
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
    
    Timer {
        id: timer
        interval: 4000
        running: mainLayout.Window.active && root.visible && !hover.hovered
        onTriggered: root.close()
    }

    contentItem: RowLayout {
        id: mainLayout
        Kirigami.Theme.colorSet: root.Kirigami.Theme.colorSet

        HoverHandler {
            id: hover
        }
        TapHandler {
            acceptedButtons: Qt.LeftButton
            onTapped: root.close()
        }
        Controls.Label {
            id: label
        }

        Controls.Button {
            id: actionButton
            visible: root.actionText.length > 0
            onClicked: {
                root.close();
                if (root.callBack) {
                    root.callBack();
                }
            }
        }
    }

    T2.Overlay.modal: Rectangle {
        color: Qt.rgba(0, 0, 0, 0.4)
    }

    T2.Overlay.modeless: Item {}

    enter: Transition {
        ParallelAnimation{
            NumberAnimation {
                target: root
                property: "position"
                easing.type: Easing.OutQuad
                duration: Kirigami.Units.longDuration
            }
            NumberAnimation {
                property: "opacity"
                from: 0.0
                to: 1.0
                easing.type: Easing.OutQuad
                duration: Kirigami.Units.longDuration
            }
        }
    }

    exit: Transition {
        NumberAnimation {
            target: root
            property: "position"
            easing.type: Easing.OutQuad
            duration: Kirigami.Units.longDuration
        }
        NumberAnimation {
            property: "opacity"
            from: 1.0
            to: 0.0
            easing.type: Easing.InQuad
            duration: Kirigami.Units.longDuration
        }
    }
}

