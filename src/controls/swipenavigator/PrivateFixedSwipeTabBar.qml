/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.10
import QtQuick.Layouts 1.12

RowLayout {
    spacing: 0

    property QtObject resizer: PrivateSwipeResizer {
        active: true
        items: {
            let ret = []
            for (let i = 0; i < fixedLayouter.count; i++) {
                let item = fixedLayouter.itemAt(i)
                ret.push(item)
            }
            return ret
        }
    }

    Repeater {
        id: fixedLayouter
        model: swipeNavigatorRoot.pages
        delegate: PrivateSwipeTab {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter
            small: true
        }
    }
}