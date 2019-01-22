/*
 *   Copyright 2019 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#pragma once

#include <QQuickItem>
#include <QVariant>
#include <QPointer>

class ContentItem;

class ColumnsView : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem *contentItem READ contentItem CONSTANT)

    Q_PROPERTY(QQmlListProperty<QQuickItem> contentChildren READ contentChildren FINAL)
    Q_CLASSINFO("DefaultProperty", "contentChildren")

    Q_ENUMS(ChildResizeMode)
public:
    enum ChildResizeMode{
        Fixed = 0,
        Dynamic,
        SingleColumn
    };
    ColumnsView(QQuickItem *parent = nullptr);
    ~ColumnsView();

    QQuickItem *contentItem() const;

    QQmlListProperty<QQuickItem> contentChildren();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

Q_SIGNALS:


private:
    static void contentChildren_append(QQmlListProperty<QQuickItem> *prop, QQuickItem *object);
    static int contentChildren_count(QQmlListProperty<QQuickItem> *prop);
    static QQuickItem *contentChildren_at(QQmlListProperty<QQuickItem> *prop, int index);
    static void contentChildren_clear(QQmlListProperty<QQuickItem> *prop);

    ContentItem *m_contentItem;
    QPointer<QQuickItem> m_currentItem;

    qreal m_oldMouseX;
    int m_currentIndex = -1;
};

