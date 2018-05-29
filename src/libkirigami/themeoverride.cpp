/*
*   Copyright (C) 2017 by Marco Martin <mart@kde.org>
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

#include "themeoverride_p.h"
#include "platformtheme.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <QPointer>
#include <QTimer>

namespace Kirigami {

class ThemeOverridePrivate {
public:
    ThemeOverridePrivate(ThemeOverride *q);
    ~ThemeOverridePrivate();

    ThemeOverride *q;
    QTimer *setColorCompressTimer;

    QColor textColor;
    QColor disabledTextColor;
    QColor highlightedTextColor;
    QColor activeTextColor;
    QColor linkColor;
    QColor visitedLinkColor;
    QColor negativeTextColor;
    QColor neutralTextColor;
    QColor positiveTextColor;

    QColor backgroundColor;
    QColor highlightColor;

    QColor focusColor;
    QColor hoverColor;

};

ThemeOverridePrivate::ThemeOverridePrivate(ThemeOverride *q)
    : q(q)
{
    setColorCompressTimer = new QTimer(q);
    setColorCompressTimer->setSingleShot(true);
    setColorCompressTimer->setInterval(0);
}

ThemeOverridePrivate::~ThemeOverridePrivate()
{}

ThemeOverride::ThemeOverride(QObject *parent)
    : QObject(parent),
      d(new ThemeOverridePrivate(this))
{
    connect(d->setColorCompressTimer, &QTimer::timeout,
            this, &ThemeOverride::colorsChanged);
}

ThemeOverride::~ThemeOverride()
{
}


QColor ThemeOverride::textColor() const
{
    return d->textColor;
}

QColor ThemeOverride::disabledTextColor() const
{
    return d->disabledTextColor;
}

QColor ThemeOverride::highlightColor() const
{
    return d->highlightColor;
}

QColor ThemeOverride::highlightedTextColor() const
{
    return d->highlightedTextColor;
}

QColor ThemeOverride::backgroundColor() const
{
    return d->backgroundColor;
}

QColor ThemeOverride::activeTextColor() const
{
    return d->activeTextColor;
}

QColor ThemeOverride::linkColor() const
{
    return d->linkColor;
}

QColor ThemeOverride::visitedLinkColor() const
{
    return d->visitedLinkColor;
}

QColor ThemeOverride::negativeTextColor() const
{
    return d->negativeTextColor;
}

QColor ThemeOverride::neutralTextColor() const
{
    return d->neutralTextColor;
}

QColor ThemeOverride::positiveTextColor() const
{
    return d->positiveTextColor;
}

QColor ThemeOverride::focusColor() const
{
    return d->focusColor;
}

QColor ThemeOverride::hoverColor() const
{
    return d->hoverColor;
}


void ThemeOverride::setTextColor(const QColor &color)
{
    if (d->textColor == color) {
        return;
    }

    d->textColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setDisabledTextColor(const QColor &color)
{
    if (d->disabledTextColor == color) {
        return;
    }

    d->disabledTextColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setBackgroundColor(const QColor &color)
{
    if (d->backgroundColor == color) {
        return;
    }

    d->backgroundColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setHighlightColor(const QColor &color)
{
    if (d->highlightColor == color) {
        return;
    }

    d->highlightColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setHighlightedTextColor(const QColor &color)
{
    if (d->highlightedTextColor == color) {
        return;
    }

    d->highlightedTextColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setActiveTextColor(const QColor &color)
{
    if (d->activeTextColor == color) {
        return;
    }

    d->activeTextColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setLinkColor(const QColor &color)
{
    if (d->linkColor == color) {
        return;
    }

    d->linkColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setVisitedLinkColor(const QColor &color)
{
    if (d->visitedLinkColor == color) {
        return;
    }

    d->visitedLinkColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setNegativeTextColor(const QColor &color)
{
    if (d->negativeTextColor == color) {
        return;
    }

    d->negativeTextColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setNeutralTextColor(const QColor &color)
{
    if (d->neutralTextColor == color) {
        return;
    }

    d->neutralTextColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setPositiveTextColor(const QColor &color)
{
    if (d->positiveTextColor == color) {
        return;
    }

    d->positiveTextColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setHoverColor(const QColor &color)
{
    if (d->hoverColor == color) {
        return;
    }

    d->hoverColor = color;
    d->setColorCompressTimer->start();
}

void ThemeOverride::setFocusColor(const QColor &color)
{
    if (d->focusColor == color) {
        return;
    }

    d->focusColor = color;
    d->setColorCompressTimer->start();
}

ThemeOverride *ThemeOverride::qmlAttachedProperties(QObject *object)
{
    //TODO: get the themeoverride
    return new ThemeOverride(object);
}

}

#include "moc_themeoverride_p.cpp"
