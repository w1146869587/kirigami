/*
 *   Copyright 2018 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.4 as Kirigami
import "../templates/private" as TemplatesPrivate

 
Kirigami.AbstractApplicationHeader {
    readonly property int leftReservedSpace: buttonsLayout.visible && buttonsLayout.visibleChildren.length > 1 ? buttonsLayout.width : 0

    height: visible ? implicitHeight : 0
    minimumHeight: globalToolBar.minimumHeight
    preferredHeight: globalToolBar.preferredHeight
    maximumHeight: globalToolBar.maximumHeight
    
    RowLayout {
        anchors.fill: parent
        spacing: 0
        RowLayout {
            id: buttonsLayout

            visible: globalToolBar.showNavigationButtons && globalToolBar.actualStyle != Kirigami.ApplicationHeaderStyle.None

            Item {
                visible: typeof applicationWindow() !== "undefined" && applicationWindow().globalDrawer.handle.handleAnchor == root
                Layout.preferredWidth: backButton.background.implicitHeight
                Layout.preferredHeight: backButton.background.implicitHeight
            }
            TemplatesPrivate.BackButton {
                id: backButton
                Layout.preferredWidth: background.implicitHeight
                Layout.preferredHeight: background.implicitHeight
            }
            TemplatesPrivate.ForwardButton {
                Layout.preferredWidth: background.implicitHeight
                Layout.preferredHeight: background.implicitHeight
            }
            Kirigami.Separator {
                Layout.preferredHeight: parent.parent.height * 0.6
                //FIXME: hacky
                opacity: buttonsLayout.visibleChildren.length > 1
            }
        }
        Loader {
            id: breadcrumbLoader
            Layout.fillWidth: true
            Layout.fillHeight: true

            active: globalToolBar.actualStyle == Kirigami.ApplicationHeaderStyle.TabBar || globalToolBar.actualStyle == Kirigami.ApplicationHeaderStyle.Breadcrumb

            //TODO: different implementation?
            sourceComponent: Kirigami.ApplicationHeader {
                minimumHeight: height
                preferredHeight: height
                maximumHeight: height
                backButtonEnabled: false
                anchors.fill:parent
                background.visible: false
                headerStyle: globalToolBar.style
            }
        }
    }
    background.visible: breadcrumbLoader.active
}
        
