/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

QtObject {
    property bool active
    onActiveChanged: relayout()

    property list<Item> items
    onItemsChanged: relayout()

    function relayout() {
        let arr = Array.from(items)
        if (active) {
            const width = Math.max(... arr.map(item => item.implicitWidth))
            arr.forEach(item => item.Layout.preferredWidth = width)
        } else {
            arr.forEach(item => item.Layout.preferredWidth = -1)
        }
    }

    property Repeater repeater: Repeater {
        model: items
        Connections {
            target: modelData
            onImplicitWidthChanged: relayout()
        }
    }
}
