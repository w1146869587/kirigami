/*
*   Copyright (C) 2018 by Marco Martin <mart@kde.org>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Library General Public License as
*   published by the Free Software Foundation; either version 2, or
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

#ifndef THEMEOVERRIDE_H
#define THEMEOVERRIDE_H

#include <QObject>
#include <QQuickItem>
#include <QColor>



namespace Kirigami {

class ThemeOverridePrivate;

/**
 * @class ThemeOverride customcolors.h ThemeOverride
 *
 * This class is the base for color management in Kirigami,
 * different platforms can reimplement this class to integrate with
 * system platform colors of a given platform
 */
class  ThemeOverride : public QObject
{
    Q_OBJECT

    // foreground colors
    /**
     * Color for normal foregrounds, usually text, but not limited to it,
     * anything that should be painted with a clear contrast should use this color
     */
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY colorsChanged)

    /**
     * Foreground color for disabled areas, usually a mid-gray
     */
    Q_PROPERTY(QColor disabledTextColor READ disabledTextColor NOTIFY colorsChanged)

    /**
     * Color for text that has been highlighted, often is a light color while normal text is dark
     */
    Q_PROPERTY(QColor highlightedTextColor READ highlightedTextColor NOTIFY colorsChanged)

    /**
     * Foreground for areas that are active or requesting attention
     */
    Q_PROPERTY(QColor activeTextColor READ activeTextColor NOTIFY colorsChanged)

    /**
     * Color for links
     */
    Q_PROPERTY(QColor linkColor READ linkColor NOTIFY colorsChanged)

    /**
     * Color for visited links, usually a bit darker than linkColor
     */
    Q_PROPERTY(QColor visitedLinkColor READ visitedLinkColor NOTIFY colorsChanged)

    /**
     * Foreground color for negative areas, such as critical error text
     */
    Q_PROPERTY(QColor negativeTextColor READ negativeTextColor NOTIFY colorsChanged)

    /**
     * Foreground color for neutral areas, such as warning texts (but not critical)
     */
    Q_PROPERTY(QColor neutralTextColor READ neutralTextColor NOTIFY colorsChanged)

    /**
     * Success messages, trusted content
     */
    Q_PROPERTY(QColor positiveTextColor READ positiveTextColor NOTIFY colorsChanged)

    //background colors
    /**
     * The generic background color
     */
    Q_PROPERTY(QColor backgroundColor READ backgroundColor NOTIFY colorsChanged)

    /**
     * The background color for selected areas
     */
    Q_PROPERTY(QColor highlightColor READ highlightColor NOTIFY colorsChanged)

    //decoration colors
    /**
     * A decoration color that indicates active focus
     */
    Q_PROPERTY(QColor focusColor READ focusColor NOTIFY colorsChanged)

    /**
     * A decoration color that indicates mouse hovering
     */
    Q_PROPERTY(QColor hoverColor READ hoverColor NOTIFY colorsChanged)

public:
    explicit ThemeOverride(QObject *parent = 0);
    ~ThemeOverride();

    //foreground colors
    QColor textColor() const;
    QColor disabledTextColor() const;
    QColor highlightedTextColor() const;
    QColor activeTextColor() const;
    QColor linkColor() const;
    QColor visitedLinkColor() const;
    QColor negativeTextColor() const;
    QColor neutralTextColor() const;
    QColor positiveTextColor() const;

    //background colors
    QColor backgroundColor() const;
    QColor highlightColor() const;
    //TODO: add active/positive/neutral/negative backgrounds?

    //decoration colors
    QColor focusColor() const;
    QColor hoverColor() const;

    //foreground colors
    void setTextColor(const QColor &color);
    void setDisabledTextColor(const QColor &color);
    void setHighlightedTextColor(const QColor &color);
    void setActiveTextColor(const QColor &color);
    void setLinkColor(const QColor &color);
    void setVisitedLinkColor(const QColor &color);
    void setNegativeTextColor(const QColor &color);
    void setNeutralTextColor(const QColor &color);
    void setPositiveTextColor(const QColor &color);

    //background colors
    void setBackgroundColor(const QColor &color);
    void setHighlightColor(const QColor &color);
    
    //decoration colors
    void setFocusColor(const QColor &color);
    void setHoverColor(const QColor &color);

//QML attached property
    static ThemeOverride *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void colorsChanged();

private:
    ThemeOverridePrivate *d;
    friend class ThemeOverridePrivate;
};

}

QML_DECLARE_TYPEINFO(Kirigami::ThemeOverride, QML_HAS_ATTACHED_PROPERTIES)

#endif // PLATFORMTHEME_H
