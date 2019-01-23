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

void ContentItem::setBoundedX(qreal x)
{
    setX(qBound(qMin(0.0, -width()+parentItem()->width()), x, 0.0));
}

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
    for (QQuickItem *child : m_items) {
        child->setSize(QSizeF(childWidth(child), height()));
        child->setPosition(QPointF(partialWidth, 0.0));
        partialWidth += child->width();
    }
    setWidth(partialWidth);

    setBoundedX((m_firstVisibleItem ? -m_firstVisibleItem->x() : 0.0));
    setY(0);
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
    setAcceptedMouseButtons(Qt::LeftButton);
}

ColumnsView::~ColumnsView()
{
}

QQuickItem *ColumnsView::contentItem() const
{
    return m_contentItem;
}

void ColumnsView::addItem(QQuickItem *item)
{
    insertItem(m_contentItem->m_items.length(), item);
}

void ColumnsView::insertItem(int pos, QQuickItem *item)
{
    if (m_contentItem->m_items.contains(item)) {
        return;
    }
    m_contentItem->m_items.insert(qBound(0, pos, m_contentItem->m_items.length()), item);
    item->setParentItem(m_contentItem);
    emit contentChildrenChanged();
}

void ColumnsView::moveItem(int from, int to)
{
    if (m_contentItem->m_items.isEmpty()
        || from < 0 || from >= m_contentItem->m_items.length()
        || to < 0 || to >= m_contentItem->m_items.length()) {
        return;
    }

    m_contentItem->m_items.move(from, to);
    m_contentItem->layoutItems();
}

void ColumnsView::removeItem(const QVariant &item)
{
    if (item.canConvert<QQuickItem *>()) {
        removeItem(item.value<QQuickItem *>());
    } else if (item.canConvert<int>()) {
        removeItem(item.toInt());
    }
}

void ColumnsView::removeItem(QQuickItem *item)
{
    if (m_contentItem->m_items.isEmpty() || !m_contentItem->m_items.contains(item)) {
        return;
    }

    m_contentItem->m_items.removeAll(item);

    if (QQmlEngine::objectOwnership(item) == QQmlEngine::JavaScriptOwnership) {
        item->deleteLater();
    } else {
        item->setParentItem(nullptr);
    }
}

void ColumnsView::removeItem(int pos)
{
    if (m_contentItem->m_items.isEmpty()
        || pos < 0 || pos >= m_contentItem->m_items.length()) {
        return;
    }

    removeItem(m_contentItem->m_items[pos]); 
}

void ColumnsView::clear()
{
    for (QQuickItem *item : m_contentItem->m_items) {
        if (QQmlEngine::objectOwnership(item) == QQmlEngine::JavaScriptOwnership) {
            item->deleteLater();
        } else {
            item->setParentItem(nullptr);
        }
    }
    m_contentItem->m_items.clear();
    emit contentChildrenChanged();
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
    m_contentItem->setBoundedX(m_contentItem->x() + event->pos().x() - m_oldMouseX);
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

    //need to make the last item visible?
    if (m_contentItem->width() - (-m_contentItem->x() + width()) < -m_contentItem->x() - firstItem->x()) {
        m_contentItem->m_firstVisibleItem = nextItem;
        m_contentItem->setBoundedX(-nextItem->x());

    //The first one found?
    } else if (-m_contentItem->x() <= firstItem->x() + firstItem->width()/2 || !nextItem) {
        m_contentItem->m_firstVisibleItem = firstItem;
        m_contentItem->setBoundedX(-firstItem->x());

    //the second?
    } else {
        m_contentItem->m_firstVisibleItem = nextItem;
        m_contentItem->setBoundedX(-nextItem->x());
    }

    event->accept();
}

void ColumnsView::contentChildren_append(QQmlListProperty<QQuickItem> *prop, QQuickItem *item)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return;
    }

    view->m_contentItem->m_items.append(item);
    item->setParentItem(view->m_contentItem);
}

int ColumnsView::contentChildren_count(QQmlListProperty<QQuickItem> *prop)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return 0;
    }

    return view->m_contentItem->m_items.count();
}

QQuickItem *ColumnsView::contentChildren_at(QQmlListProperty<QQuickItem> *prop, int index)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return nullptr;
    }

    if (index < 0 || index >= view->m_contentItem->m_items.count()) {
        return nullptr;
    }
    return view->m_contentItem->m_items.value(index);
}

void ColumnsView::contentChildren_clear(QQmlListProperty<QQuickItem> *prop)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return;
    }

    return view->m_contentItem->m_items.clear();
}

QQmlListProperty<QQuickItem> ColumnsView::contentChildren()
{
    return QQmlListProperty<QQuickItem>(this, nullptr,
                                     contentChildren_append,
                                     contentChildren_count,
                                     contentChildren_at,
                                     contentChildren_clear);
}


#include "moc_columnsview.cpp"
