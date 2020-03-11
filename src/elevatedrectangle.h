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

#ifndef ELEVATEDRECTANGLE_H
#define ELEVATEDRECTANGLE_H

#include <memory>
#include <QQuickItem>

class ElevatedRectangle : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal xOffset READ xOffset WRITE setXOffset NOTIFY xOffsetChanged)
    Q_PROPERTY(qreal yOffset READ yOffset WRITE setYOffset NOTIFY yOffsetChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor shadowColor READ shadowColor WRITE setShadowColor NOTIFY shadowColorChanged)

public:
    ElevatedRectangle(QQuickItem *parent = nullptr);
    ~ElevatedRectangle() override;

    qreal size() const;
    void setSize(qreal newSize);
    Q_SIGNAL void sizeChanged();

    qreal radius() const;
    void setRadius(qreal newRadius);
    Q_SIGNAL void radiusChanged();

    qreal xOffset() const;
    void setXOffset(qreal newXOffset);
    Q_SIGNAL void xOffsetChanged();

    qreal yOffset() const;
    void setYOffset(qreal newYOffset);
    Q_SIGNAL void yOffsetChanged();

    QColor color() const;
    void setColor(const QColor &newColor);
    Q_SIGNAL void colorChanged();

    QColor shadowColor() const;
    void setShadowColor(const QColor &newShadowColor);
    Q_SIGNAL void shadowColorChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data) override;

private:
    class Private;
    const std::unique_ptr<Private> d;
};

#endif // ELEVATEDRECTANGLE_H
