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

    
}

