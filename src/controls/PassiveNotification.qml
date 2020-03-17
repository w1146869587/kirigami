/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import org.kde.kirigami 2.12 as Kirigami
import "templates" as KT

KT.PassiveNotification {
    id: root

    x: parent.width/2 - width/2
    y: parent.height - height - Kirigami.Units.largeSpacing

    background: Kirigami.ShadowedRectangle {
        Kirigami.Theme.colorSet: root.Kirigami.Theme.colorSet
        shadow {
            size: Kirigami.Units.gridUnit/2
            color: Qt.rgba(0, 0, 0, 0.4) 
            yOffset: 2
        }
        radius: Kirigami.Units.smallSpacing * 2
        color: Kirigami.Theme.backgroundColor
        opacity: 0.6
    }
}

