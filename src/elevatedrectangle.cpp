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

#include "elevatedrectangle.h"

#include "scenegraph/elevatedrectanglenode.h"

class ElevatedRectangle::Private
{
public:
    qreal elevation = 0.0;
    qreal radius = 0.0;
    qreal xOffset = 0.0;
    qreal yOffset = 0.0;
    QColor color = Qt::white;
    QColor shadowColor = Qt::black;
};

ElevatedRectangle::ElevatedRectangle(QQuickItem *parentItem)
    : QQuickItem(parentItem), d(new Private)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

ElevatedRectangle::~ElevatedRectangle()
{
}

qreal ElevatedRectangle::elevation() const
{
    return d->elevation;
}

void ElevatedRectangle::setElevation(qreal newElevation)
{
    if (newElevation == d->elevation) {
        return;
    }

    d->elevation = newElevation;
    update();
    Q_EMIT elevationChanged();
}

qreal ElevatedRectangle::radius() const
{
    return d->radius;
}

void ElevatedRectangle::setRadius(qreal newRadius)
{
    if (newRadius == d->radius) {
        return;
    }

    d->radius = newRadius;
    update();
    Q_EMIT radiusChanged();
}

qreal ElevatedRectangle::xOffset() const
{
    return d->xOffset;
}

void ElevatedRectangle::setXOffset(qreal newXOffset)
{
    if (newXOffset == d->xOffset) {
        return;
    }

    d->xOffset = newXOffset;
    update();
    Q_EMIT xOffsetChanged();
}

qreal ElevatedRectangle::yOffset() const
{
    return d->yOffset;
}

void ElevatedRectangle::setYOffset(qreal newYOffset)
{
    if (newYOffset == d->yOffset) {
        return;
    }

    d->yOffset = newYOffset;
    update();
    Q_EMIT yOffsetChanged();
}

QColor ElevatedRectangle::color() const
{
    return d->color;
}

void ElevatedRectangle::setColor(const QColor & newColor)
{
    if (newColor == d->color) {
        return;
    }

    d->color = newColor;
    update();
    Q_EMIT colorChanged();
}

QColor ElevatedRectangle::shadowColor() const
{
    return d->shadowColor;
}

void ElevatedRectangle::setShadowColor(const QColor &newShadowColor)
{
    if (newShadowColor == d->shadowColor) {
        return;
    }

    d->shadowColor = newShadowColor;
    update();
    Q_EMIT shadowColorChanged();
}

QSGNode *ElevatedRectangle::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    if (!node) {
        node = new ElevatedRectangleNode;
    }

    auto elevatedNode = static_cast<ElevatedRectangleNode*>(node);
    elevatedNode->setRect(boundingRect());
    elevatedNode->setElevation(d->elevation);
    elevatedNode->setRadius(d->radius);
    elevatedNode->setOffset(QVector2D{float(d->xOffset), float(d->yOffset)});
    elevatedNode->setColor(d->color);
    elevatedNode->setShadowColor(d->shadowColor);
    elevatedNode->updateGeometry();

    return elevatedNode;
}
