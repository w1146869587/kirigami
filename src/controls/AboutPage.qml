/*
 *   Copyright (C) 2018 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.5

/**
 * An about page that is ready to integrate in a kirigami app
 *
 * Allows to have a page that will show the copyright notice of the application
 * together with the contributors and some information of which platform it's
 * running on.
 *
 * @since 5.52
 * @since org.kde.kirigami 2.6
 */
ScrollablePage
{
    id: page
    /**
     * An object with the same shape of KAboutData
     *
     * For example:
     * @code
     * aboutData: {
        "displayName" : "KirigamiApp",
        "productName" : "kirigami/app",
        "componentName" : "kirigamiapp",
        "shortDescription" : "A Kirigami example",
        "homepage" : "",
        "bugAddress" : "submit@bugs.kde.org",
        "version" : "5.14.80",
        "otherText" : "",
        "authors" : [
            {
                "name" : "...",
                "task" : "",
                "emailAddress" : "somebody@kde.org",
                "webAddress" : "",
                "ocsUsername" : ""
            }
        ],
        "credits" : [],
        "translators" : [],
        "licenses" : [
            {
                "name" : "GPL v2",
                "text" : "long, boring, license text",
                "spdx" : "GPL-2.0"
            }
        ],
        "copyrightStatement" : "© 2010-2018 Plasma Development Team",
        "desktopFileName" : "org.kde.kirigamiapp"
        }
        @endcode
     *
     * @see KAboutData
     */
    property var aboutData

    title: i18n("About")
    Component {
        id: licencePage
        ScrollablePage {
            property alias text: content.text
            QQC2.TextArea {
                id: content
                readOnly: true
            }
        }
    }

    Component {
        id: personDelegate
        AbstractCard {
            Layout.fillWidth: true
            contentItem: RowLayout {
                Layout.preferredHeight: Units.iconSizes.medium
                Icon {
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignVCenter
                    Layout.minimumWidth: Units.iconSizes.medium
                    Layout.maximumWidth: Units.iconSizes.medium
                    source: "user" //TODO: use ocs once we don't need to depend on attica, was using gravatar but it didn't feel right
                    fallback: "user"
                }
                QQC2.Label {
                    Layout.fillWidth: true
                    text: i18n("%1 <%2>", modelData.name, modelData.emailAddress)
                }
            }
        }
    }

    ColumnLayout {
        GridLayout {
            columns: 2
            Layout.fillWidth: true
            Layout.preferredHeight: Units.iconSizes.huge

            Icon {
                Layout.rowSpan: 2
                Layout.fillHeight: true
                Layout.minimumWidth: height
                Layout.rightMargin: Units.largeSpacing
                source: page.aboutData.programLogo || page.aboutData.programIconName
            }
            Heading {
                Layout.fillWidth: true
                text: page.aboutData.displayName + " " + page.aboutData.version
            }
            Heading {
                Layout.fillWidth: true
                level: 2
                text: page.aboutData.shortDescription
            }
        }

        Separator {
            Layout.fillWidth: true
        }

        Heading {
            text: i18n("Copyright")
        }
        QQC2.Label {
            Layout.leftMargin: Units.gridUnit
            text: aboutData.otherText
            visible: text.length > 0
        }
        QQC2.Label {
            Layout.leftMargin: Units.gridUnit
            text: aboutData.copyrightStatement
            visible: text.length > 0
        }
        UrlButton {
            Layout.leftMargin: Units.gridUnit
            url: aboutData.homepage
            visible: url.length > 0
        }

        Repeater {
            model: aboutData.licenses
            delegate: RowLayout {
                Layout.leftMargin: Units.gridUnit
                QQC2.Label { text: i18n("License:" ) }
                LinkButton {
                    text: modelData.name
                    onClicked: applicationWindow().pageStack.push(licencePage, { text: modelData.text, title: modelData.name } )
                }
            }
        }

        Heading {
            text: i18n("Libraries in use")
            visible: Settings.information
        }
        Repeater {
            model: Settings.information
            delegate: QQC2.Label {
                Layout.leftMargin: Units.gridUnit
                id: libraries
                text: modelData
            }
        }
        Heading {
            Layout.fillWidth: true
            text: i18n("Authors")
        }

        Repeater {
            model: aboutData.authors
            delegate: personDelegate
        }
        Heading {
            text: i18n("Credits")
            visible: repCredits.count > 0
        }
        Repeater {
            id: repCredits
            model: aboutData.translators
            delegate: personDelegate
        }
        Heading {
            text: i18n("Translators")
            visible: repTranslators.count > 0
        }
        Repeater {
            id: repTranslators
            model: aboutData.translators
            delegate: personDelegate
        }
    }
}
