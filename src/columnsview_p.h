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

#include "columnsview.h"

#include <QQuickItem>
#include <QPointer>


class ContentItem : public QQuickItem {
    Q_OBJECT

public:
    ContentItem(QQuickItem *parent = nullptr);
    ~ContentItem();

    qreal childWidth(QQuickItem *child);
    void layoutItems();

protected:
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;

private:
    QPointer<QQuickItem> m_firstVisibleItem;
    QPointer<QQuickItem> m_expandedItem;

    qreal m_columnWidth = 200;
    int m_reservedColumns = 1;
    ColumnsView::ChildResizeMode m_resizeMode = ColumnsView::Fixed;
    friend class ColumnsView;
};

