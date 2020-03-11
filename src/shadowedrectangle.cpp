/*
 * Copyright 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "shadowedrectangle.h"

#include "scenegraph/shadowedrectanglenode.h"

class ShadowedRectangle::Private
{
public:
    qreal size = 0.0;
    qreal radius = 0.0;
    qreal xOffset = 0.0;
    qreal yOffset = 0.0;
    QColor color = Qt::white;
    QColor shadowColor = Qt::black;
};

ShadowedRectangle::ShadowedRectangle(QQuickItem *parentItem)
    : QQuickItem(parentItem), d(new Private)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

ShadowedRectangle::~ShadowedRectangle()
{
}

qreal ShadowedRectangle::size() const
{
    return d->size;
}

void ShadowedRectangle::setSize(qreal newSize)
{
    if (newSize == d->size) {
        return;
    }

    d->size = newSize;
    update();
    Q_EMIT sizeChanged();
}

qreal ShadowedRectangle::radius() const
{
    return d->radius;
}

void ShadowedRectangle::setRadius(qreal newRadius)
{
    if (newRadius == d->radius) {
        return;
    }

    d->radius = newRadius;
    update();
    Q_EMIT radiusChanged();
}

qreal ShadowedRectangle::xOffset() const
{
    return d->xOffset;
}

void ShadowedRectangle::setXOffset(qreal newXOffset)
{
    if (newXOffset == d->xOffset) {
        return;
    }

    d->xOffset = newXOffset;
    update();
    Q_EMIT xOffsetChanged();
}

qreal ShadowedRectangle::yOffset() const
{
    return d->yOffset;
}

void ShadowedRectangle::setYOffset(qreal newYOffset)
{
    if (newYOffset == d->yOffset) {
        return;
    }

    d->yOffset = newYOffset;
    update();
    Q_EMIT yOffsetChanged();
}

QColor ShadowedRectangle::color() const
{
    return d->color;
}

void ShadowedRectangle::setColor(const QColor & newColor)
{
    if (newColor == d->color) {
        return;
    }

    d->color = newColor;
    update();
    Q_EMIT colorChanged();
}

QColor ShadowedRectangle::shadowColor() const
{
    return d->shadowColor;
}

void ShadowedRectangle::setShadowColor(const QColor &newShadowColor)
{
    if (newShadowColor == d->shadowColor) {
        return;
    }

    d->shadowColor = newShadowColor;
    update();
    Q_EMIT shadowColorChanged();
}

QSGNode *ShadowedRectangle::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    if (!node) {
        node = new ShadowedRectangleNode;
    }

    auto elevatedNode = static_cast<ShadowedRectangleNode*>(node);
    elevatedNode->setRect(boundingRect());
    elevatedNode->setSize(d->size);
    elevatedNode->setRadius(d->radius);
    elevatedNode->setOffset(QVector2D{float(d->xOffset), float(d->yOffset)});
    elevatedNode->setColor(d->color);
    elevatedNode->setShadowColor(d->shadowColor);
    elevatedNode->updateGeometry();

    return elevatedNode;
}
