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

#include "platformtheme.h"
#include "kirigamipluginfactory.h"
#include "basictheme_p.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QGuiApplication>
#include <QDebug>
#include <QPointer>
#include <QQuickWindow>
#include <QPluginLoader>
#include <QDir>
#include <QTimer>
#include <QQuickStyle>

namespace Kirigami {

class PlatformThemePrivate {
public:
    PlatformThemePrivate(PlatformTheme *q);
    ~PlatformThemePrivate();

    void findParentStyle();
    static QColor tint(const QColor &c1, const QColor &c2, qreal ratio);


    PlatformTheme *q;
    QTimer *setColorCompressTimer;
    PlatformTheme::ColorSet m_colorSet = PlatformTheme::Window;
    PlatformTheme::ColorGroup m_colorGroup = PlatformTheme::Active;
    QSet<PlatformTheme *> m_childThemes;
    QPointer<PlatformTheme> m_parentTheme;

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

    QFont font;
    QPalette palette;
    bool m_inherit = true;
    bool m_init = true;
};

PlatformThemePrivate::PlatformThemePrivate(PlatformTheme *q)
    : q(q)
{
    setColorCompressTimer = new QTimer(q);
    setColorCompressTimer->setSingleShot(true);
    setColorCompressTimer->setInterval(0);
}

PlatformThemePrivate::~PlatformThemePrivate()
{}

void PlatformThemePrivate::findParentStyle()
{
    if (m_parentTheme) {
        m_parentTheme->d->m_childThemes.remove(q);
    }
    QQuickItem *candidate = qobject_cast<QQuickItem *>(q->parent());
    while (candidate) {
        candidate = candidate->parentItem();
        PlatformTheme *t = static_cast<PlatformTheme *>(qmlAttachedPropertiesObject<PlatformTheme>(candidate, false));
        if (t) {
            t->d->m_childThemes.insert(q);
            m_parentTheme = t;
            if (m_inherit) {
                q->setColorSet(t->colorSet());
                if (m_colorSet == PlatformTheme::Custom) {
                    q->setTextColor(t->textColor());
                    q->setDisabledTextColor(t->disabledTextColor());
                    q->setHighlightedTextColor(t->highlightedTextColor());
                    q->setActiveTextColor(t->activeTextColor());
                    q->setLinkColor(t->linkColor());
                    q->setVisitedLinkColor(t->visitedLinkColor());
                    q->setNegativeTextColor(t->negativeTextColor());
                    q->setNeutralTextColor(t->neutralTextColor());
                    q->setPositiveTextColor(t->positiveTextColor());
                    q->setBackgroundColor(t->backgroundColor());
                    q->setHighlightColor(t->highlightColor());
                    q->setFocusColor(t->focusColor());
                    q->setHoverColor(t->hoverColor());

                    q->setPalette(t->palette());

                }
            }
            break;
        }
        
    }
}

QColor PlatformThemePrivate::tint(const QColor &c1, const QColor &c2, qreal ratio)
{
    qreal r = c1.redF() + (c2.redF() - c1.redF()) * ratio;
    qreal g = c1.greenF() + (c2.greenF() - c1.greenF()) * ratio;
    qreal b = c1.blueF() + (c2.blueF() - c1.blueF()) * ratio;
 
    return QColor::fromRgbF(r, g, b, 1);
}




PlatformTheme::PlatformTheme(QObject *parent)
    : QObject(parent),
      d(new PlatformThemePrivate(this))
{
    connect(d->setColorCompressTimer, &QTimer::timeout,
            this, &PlatformTheme::colorsChanged);
    d->findParentStyle();

    if (QQuickItem *item = qobject_cast<QQuickItem *>(parent)) {
        connect(item, &QQuickItem::windowChanged, this, [this]() {
            d->findParentStyle();
        });
        connect(item, &QQuickItem::parentChanged, this, [this]() {
            d->findParentStyle();
        });
    }
    d->m_init = false;
    //TODO: needs https://codereview.qt-project.org/#/c/206889/ for font changes
}

PlatformTheme::~PlatformTheme()
{
    if (d->m_parentTheme) {
        d->m_parentTheme->d->m_childThemes.remove(this);
    }
}

void PlatformTheme::setColorSet(PlatformTheme::ColorSet colorSet)
{
    if (d->m_colorSet == colorSet) {
        return;
    }

    d->m_colorSet = colorSet;

    for (PlatformTheme *t : d->m_childThemes) {
        if (t->inherit()) {
            t->setColorSet(colorSet);
            if (colorSet == Custom) {
                t->setTextColor(d->textColor);
                t->setDisabledTextColor(d->disabledTextColor);
                t->setHighlightedTextColor(d->highlightedTextColor);
                t->setActiveTextColor(d->activeTextColor);
                t->setLinkColor(d->linkColor);
                t->setVisitedLinkColor(d->visitedLinkColor);
                t->setNegativeTextColor(d->negativeTextColor);
                t->setNeutralTextColor(d->neutralTextColor);
                t->setPositiveTextColor(d->positiveTextColor);
                t->setBackgroundColor(d->backgroundColor);
                t->setHighlightColor(d->highlightColor);
                t->setFocusColor(d->focusColor);
                t->setHoverColor(d->hoverColor);

                t->setPalette(d->palette);
            }
        }
    }

    if (!d->m_init) {
        emit colorSetChanged(colorSet);
        d->setColorCompressTimer->start();
    }
}

PlatformTheme::ColorSet PlatformTheme::colorSet() const
{
    return d->m_colorSet;
}

void PlatformTheme::setColorGroup(PlatformTheme::ColorGroup colorGroup)
{
    if (d->m_colorGroup == colorGroup) {
        return;
    }

    d->m_colorGroup = colorGroup;

    for (PlatformTheme *t : d->m_childThemes) {
        if (t->inherit()) {
            t->setColorGroup(colorGroup);
        }
    }

    if (!d->m_init) {
        emit colorGroupChanged(colorGroup);
        d->setColorCompressTimer->start();
    }
}

PlatformTheme::ColorGroup PlatformTheme::colorGroup() const
{
    return d->m_colorGroup;
}

bool PlatformTheme::inherit() const
{
    return d->m_inherit;
}

void PlatformTheme::setInherit(bool inherit)
{
    if (d->m_inherit == inherit) {
        return;
    }

    d->m_inherit = inherit;
    if (inherit && d->m_parentTheme) {
        setColorSet(d->m_parentTheme->colorSet());
    }
    emit inheritChanged(inherit);
}

QColor PlatformTheme::textColor() const
{
    return d->textColor;
}

QColor PlatformTheme::disabledTextColor() const
{
    return d->disabledTextColor;
}

QColor PlatformTheme::highlightColor() const
{
    return d->highlightColor;
}

QColor PlatformTheme::highlightedTextColor() const
{
    return d->highlightedTextColor;
}

QColor PlatformTheme::backgroundColor() const
{
    return d->backgroundColor;
}

QColor PlatformTheme::activeTextColor() const
{
    return d->activeTextColor;
}

QColor PlatformTheme::linkColor() const
{
    return d->linkColor;
}

QColor PlatformTheme::visitedLinkColor() const
{
    return d->visitedLinkColor;
}

QColor PlatformTheme::negativeTextColor() const
{
    return d->negativeTextColor;
}

QColor PlatformTheme::neutralTextColor() const
{
    return d->neutralTextColor;
}

QColor PlatformTheme::positiveTextColor() const
{
    return d->positiveTextColor;
}

QColor PlatformTheme::focusColor() const
{
    return d->focusColor;
}

QColor PlatformTheme::hoverColor() const
{
    return d->hoverColor;
}

#define PROPAGATECUSTOMCOLOR(colorName, color)\
          if (colorSet() == Custom) {\
              for (PlatformTheme *t : d->m_childThemes) {\
                  if (t->inherit() && t->colorSet() == Custom) {\
                      t->set##colorName(color);\
                  }\
              }\
          }

void PlatformTheme::setTextColor(const QColor &color)
{
    if (d->textColor == color) {
        return;
    }

    d->textColor = color;
    PROPAGATECUSTOMCOLOR(TextColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setDisabledTextColor(const QColor &color)
{
    if (d->disabledTextColor == color) {
        return;
    }

    d->disabledTextColor = color;
    PROPAGATECUSTOMCOLOR(DisabledTextColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setBackgroundColor(const QColor &color)
{
    if (d->backgroundColor == color) {
        return;
    }

    d->backgroundColor = color;
    PROPAGATECUSTOMCOLOR(BackgroundColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setHighlightColor(const QColor &color)
{
    if (d->highlightColor == color) {
        return;
    }

    d->highlightColor = color;
    PROPAGATECUSTOMCOLOR(HighlightColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setHighlightedTextColor(const QColor &color)
{
    if (d->highlightedTextColor == color) {
        return;
    }

    d->highlightedTextColor = color;
    PROPAGATECUSTOMCOLOR(HighlightedTextColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setActiveTextColor(const QColor &color)
{
    if (d->activeTextColor == color) {
        return;
    }

    d->activeTextColor = color;
    PROPAGATECUSTOMCOLOR(ActiveTextColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setLinkColor(const QColor &color)
{
    if (d->linkColor == color) {
        return;
    }

    d->linkColor = color;
    PROPAGATECUSTOMCOLOR(LinkColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setVisitedLinkColor(const QColor &color)
{
    if (d->visitedLinkColor == color) {
        return;
    }

    d->visitedLinkColor = color;
    PROPAGATECUSTOMCOLOR(VisitedLinkColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setNegativeTextColor(const QColor &color)
{
    if (d->negativeTextColor == color) {
        return;
    }

    d->negativeTextColor = color;
    PROPAGATECUSTOMCOLOR(NegativeTextColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setNeutralTextColor(const QColor &color)
{
    if (d->neutralTextColor == color) {
        return;
    }

    d->neutralTextColor = color;
    PROPAGATECUSTOMCOLOR(NeutralTextColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setPositiveTextColor(const QColor &color)
{
    if (d->positiveTextColor == color) {
        return;
    }

    d->positiveTextColor = color;
    PROPAGATECUSTOMCOLOR(PositiveTextColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setHoverColor(const QColor &color)
{
    if (d->hoverColor == color) {
        return;
    }

    d->hoverColor = color;
    PROPAGATECUSTOMCOLOR(HoverColor, color)
    d->setColorCompressTimer->start();
}

void PlatformTheme::setFocusColor(const QColor &color)
{
    if (d->focusColor == color) {
        return;
    }

    d->focusColor = color;
    PROPAGATECUSTOMCOLOR(FocusColor, color)
    d->setColorCompressTimer->start();
}

QFont PlatformTheme::defaultFont() const
{
    return d->font;
}

void PlatformTheme::setDefaultFont(const QFont &font)
{
    if (d->font == font) {
        return;
    }

    d->font = font;
    emit defaultFontChanged(font);
}

QPalette PlatformTheme::palette() const
{
    return d->palette;
}

void PlatformTheme::setPalette(const QPalette &palette)
{
    if (d->palette == palette) {
        return;
    }

    d->palette = palette;
    PROPAGATECUSTOMCOLOR(Palette, palette)
    emit paletteChanged(palette);
}

QIcon PlatformTheme::iconFromTheme(const QString &name, const QColor &customColor)
{
    QIcon icon = QIcon::fromTheme(name);
    if (!icon.isNull() && (name.endsWith("-symbolic") || customColor != Qt::transparent)) {
        icon.setIsMask(true);
    }
    return icon;
}



PlatformTheme *PlatformTheme::qmlAttachedProperties(QObject *object)
{
    for (const QString &path : QCoreApplication::libraryPaths()) {
        QDir dir(path + "/kf5/kirigami");
        for (const QString &fileName : dir.entryList(QDir::Files)) {
            //TODO: env variable?
            if (fileName.startsWith(QQuickStyle::name())) {
                QPluginLoader loader(dir.absoluteFilePath(fileName));
                QObject *plugin = loader.instance();
                //TODO: load actually a factory as plugin

                KirigamiPluginFactory *factory = qobject_cast<KirigamiPluginFactory *>(plugin);
                if (factory) {
                    return factory->createPlatformTheme(object);
                }
            }
        }
    }

    return new BasicTheme(object);
}

}

#include "moc_platformtheme.cpp"
