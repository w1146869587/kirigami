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

#pragma once

#include <QSGGeometryNode>
#include <QColor>
#include <QVector2D>

class ShadowedRectangleMaterial;

class ShadowedRectangleNode : public QSGGeometryNode
{
public:
    ShadowedRectangleNode(const QRectF &rect = QRectF{});

    void setRect(const QRectF &rect);
    void setSize(qreal size);
    void setRadius(qreal radius);
    void setColor(const QColor &color);
    void setShadowColor(const QColor &color);
    void setOffset(const QVector2D &offset);

    void updateGeometry();

private:
    QSGGeometry *m_geometry;
    ShadowedRectangleMaterial *m_material;

    QRectF m_rect;
    qreal m_size = 0.0;
    qreal m_radius = 0.0;
    QVector2D m_offset;
    QVector2D m_aspect;
};
