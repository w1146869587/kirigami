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

#include "columnsview.h"
#include "columnsview_p.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>

ContentItem::ContentItem(QQuickItem *parent)
    : QQuickItem(parent)
{}

ContentItem::~ContentItem()
{}

qreal ContentItem::childWidth(QQuickItem *child)
{
    if (m_resizeMode == ColumnsView::SingleColumn
        || parentItem()->width() < m_columnWidth * 2) {
        return parentItem()->width();

    } else if (m_resizeMode == ColumnsView::Fixed) {
        if (child == m_expandedItem) {
            return qBound(m_columnWidth, (parentItem()->width() - m_columnWidth * m_reservedColumns), parentItem()->width());
        } else {
            return qMin(parentItem()->width(), m_columnWidth);
        }

    } else {
        //TODO:look for Layout size hints
        if (child->implicitWidth() > 0) {
            return qMin(parentItem()->width(), child->implicitWidth());
        }
        return qMin(parentItem()->width(), child->width());
    }
}

void ContentItem::layoutItems()
{
    qreal partialWidth = 0;
    for (QQuickItem *child : childItems()) {
        child->setSize(QSizeF(childWidth(child), height()));
        child->setPosition(QPointF(partialWidth, 0.0));
        partialWidth += child->width();
    }
    setWidth(partialWidth);
    
    setPosition(QPointF(m_firstVisibleItem ? -m_firstVisibleItem->x() : 0.0, 0.0));
}

void ContentItem::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    switch (change) {
    case QQuickItem::ItemChildAddedChange:
        connect(value.item, &QQuickItem::widthChanged, this, &ContentItem::layoutItems);
        layoutItems();
        break;
    case QQuickItem::ItemChildRemovedChange:
        disconnect(value.item, nullptr, this, nullptr);
        layoutItems();
        break;
    default:
        break;
    }
    QQuickItem::itemChange(change, value);
}







ColumnsView::ColumnsView(QQuickItem *parent)
    : QQuickItem(parent)
{
    m_contentItem = new ContentItem(this);
}

ColumnsView::~ColumnsView()
{
}

QQuickItem *ColumnsView::contentItem() const
{
    return m_contentItem;
}

void ColumnsView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_UNUSED(oldGeometry);

    m_contentItem->layoutItems();
    m_contentItem->setHeight(newGeometry.height());
}

void ColumnsView::mousePressEvent(QMouseEvent *event)
{
    m_oldMouseX = event->localPos().x();
    event->accept();
}

void ColumnsView::mouseMoveEvent(QMouseEvent *event)
{
    m_contentItem->setX(m_contentItem->x() + event->pos().x() - m_oldMouseX);
    m_oldMouseX = event->pos().x();
    event->accept();
}

void ColumnsView::mouseReleaseEvent(QMouseEvent *event)
{
    //TODO: animate
    QQuickItem *firstItem = m_contentItem->childAt(-m_contentItem->x(), 0);
    if (!firstItem) {
        return;
    }
    QQuickItem *nextItem = m_contentItem->childAt(firstItem->x() + firstItem->width() + 1, 0);

    if (-m_contentItem->x() <= firstItem->x() + firstItem->width()/2 || !nextItem) {
        m_contentItem->m_expandedItem = firstItem;
        m_contentItem->setX(-firstItem->x());
    } else {
        m_contentItem->m_expandedItem = nextItem;
        m_contentItem->setX(-nextItem->x());
    }

    event->accept();
}


#include "moc_columnsview.cpp"
