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
 * License along with this library.  If not, see <https://www.gnu.org/licenses/>
 */

#include "elevatedrectanglenode.h"

#include "elevatedrectanglematerial.h"

#include <QDebug>

ElevatedRectangleNode::ElevatedRectangleNode(const QRectF& rect)
{
    m_geometry = new QSGGeometry{QSGGeometry::defaultAttributes_TexturedPoint2D(), 4};
    setGeometry(m_geometry);
    setRect(rect);

    m_material = new ElevatedRectangleMaterial{};
    setMaterial(m_material);

    setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial);
}

void ElevatedRectangleNode::setRect(const QRectF& rect)
{
    if (rect == m_rect) {
        return;
    }

    m_rect = rect;

    QVector2D newAspect{1.0, 1.0};
    if (m_rect.width() >= m_rect.height()) {
        newAspect.setX(m_rect.width() / m_rect.height());
    } else {
        newAspect.setY(m_rect.height() / m_rect.width());
    }
    if (m_material->aspect != newAspect) {
        m_material->aspect = newAspect;
        markDirty(QSGNode::DirtyMaterial);
        m_aspect = newAspect;
    }
}

void ElevatedRectangleNode::setSize(qreal size)
{
    auto minDimension = std::min(m_rect.width(), m_rect.height());
    float uniformSize = (size / minDimension) * 2.0;

    if (!qFuzzyCompare(m_material->size, uniformSize)) {
        m_material->size = uniformSize;
        markDirty(QSGNode::DirtyMaterial);
        m_size = size;
    }
}

void ElevatedRectangleNode::setRadius(qreal radius)
{
    auto minDimension = std::min(m_rect.width(), m_rect.height());
    float uniformRadius = radius * 2.0 / minDimension;

    if (!qFuzzyCompare(m_material->radius, uniformRadius)) {
        m_material->radius = std::min(uniformRadius, 1.0f);
        markDirty(QSGNode::DirtyMaterial);
        m_radius = radius;
    }
}

void ElevatedRectangleNode::setColor(const QColor &color)
{
    if (m_material->color != color) {
        m_material->color = color;
        markDirty(QSGNode::DirtyMaterial);
    }
}

void ElevatedRectangleNode::setShadowColor(const QColor& color)
{
    if (m_material->shadowColor != color) {
        m_material->shadowColor = color;
        markDirty(QSGNode::DirtyMaterial);
    }
}

void ElevatedRectangleNode::setOffset(const QVector2D& offset)
{
    auto minDimension = std::min(m_rect.width(), m_rect.height());
    auto uniformOffset = offset * 2.0 / minDimension;

    if (m_material->offset != uniformOffset) {
        m_material->offset = uniformOffset;
        markDirty(QSGNode::DirtyMaterial);
        m_offset = offset;
    }
}

void ElevatedRectangleNode::updateGeometry()
{
    auto rect = m_rect.adjusted(-m_size * m_aspect.x(), -m_size * m_aspect.y(),
                                m_size * m_aspect.x(), m_size * m_aspect.y());

    auto offsetLength = m_offset.length();

    rect = rect.adjusted(-offsetLength * m_aspect.x(), -offsetLength * m_aspect.y(),
                         offsetLength * m_aspect.x(), offsetLength * m_aspect.y());

    QSGGeometry::updateTexturedRectGeometry(m_geometry, rect, QRectF{0.0, 0.0, 1.0, 1.0});
    markDirty(QSGNode::DirtyGeometry);
}


