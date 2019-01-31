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

#include <QGuiApplication>
#include <QStyleHints>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>
#include <QPropertyAnimation>


QHash<QObject *, ColumnsViewAttached *> ColumnsView::m_attachedObjects = QHash<QObject *, ColumnsViewAttached *>();

ColumnsViewAttached::ColumnsViewAttached(QObject *parent)
    : QObject(parent)
{}

ColumnsViewAttached::~ColumnsViewAttached()
{}

void ColumnsViewAttached::setLevel(int level)
{
    if (!m_customFillWidth && m_view) {
        m_fillWidth = level == m_view->depth() - 1;
    }

    if (level == m_level) {
        return;
    }

    m_level = level;
    emit levelChanged();
}

int ColumnsViewAttached::level() const
{
    return m_level;
}

void ColumnsViewAttached::setFillWidth(bool fill)
{
    if (m_view) {
        disconnect(m_view.data(), &ColumnsView::depthChanged, this, nullptr);
    }
    m_customFillWidth = true;

    if (fill == m_fillWidth) {
        return;
    }

    m_fillWidth = fill;
    emit fillWidthChanged();
}

bool ColumnsViewAttached::fillWidth() const
{
    return m_fillWidth;
}

qreal ColumnsViewAttached::reservedSpace() const
{
    return m_reservedSpace;
}

void ColumnsViewAttached::setReservedSpace(qreal space)
{
    if (m_view) {
        disconnect(m_view.data(), &ColumnsView::columnWidthChanged, this, nullptr);
    }
    m_customReservedSpace = true;

    if (qFuzzyCompare(space, m_reservedSpace)) {
        return;
    }

    m_reservedSpace = space;
    emit reservedSpaceChanged();
}

ColumnsView *ColumnsViewAttached::view()
{
    return m_view;
}

void ColumnsViewAttached::setView(ColumnsView *view)
{
    if (view == m_view) {
        return;
    }

    if (m_view) {
        disconnect(m_view.data(), nullptr, this, nullptr);
    }
    m_view = view;

    if (!m_customFillWidth && m_view) {
        m_fillWidth = m_level == m_view->depth() - 1;
        connect(m_view.data(), &ColumnsView::depthChanged, this, [this]() {
            m_fillWidth = m_level == m_view->depth() - 1;
            emit fillWidthChanged();
        });
    }
    if (!m_customReservedSpace && m_view) {
        m_reservedSpace = m_view->columnWidth();
        connect(m_view.data(), &ColumnsView::columnWidthChanged, this, [this]() {
            m_reservedSpace = m_view->columnWidth();
            emit reservedSpaceChanged();
        });
    }

    emit viewChanged();
}



ContentItem::ContentItem(ColumnsView *parent)
    : QQuickItem(parent),
      m_view(parent)
{
    m_slideAnim = new QPropertyAnimation(this);
    m_slideAnim->setTargetObject(this);
    m_slideAnim->setPropertyName("x");
    //TODO: from Units
    m_slideAnim->setDuration(250);
    m_slideAnim->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    connect(m_slideAnim, &QPropertyAnimation::finished, this, [this] () {
        if (!m_view->currentItem()) {
            m_view->setCurrentIndex(m_items.indexOf(m_viewAnchorItem));
        }
        // If the current item is not on view, change it
        QRectF mapped = m_view->currentItem()->mapRectToItem(parentItem(), QRectF(m_view->currentItem()->position(), m_view->currentItem()->size()));
        if (!QRectF(QPointF(0, 0), size()).intersects(mapped)) {
            m_view->setCurrentIndex(m_items.indexOf(m_viewAnchorItem));
        }
    });
}

ContentItem::~ContentItem()
{}

void ContentItem::setBoundedX(qreal x)
{
    if (!parentItem()) {
        return;
    }
    m_slideAnim->stop();
    setX(qBound(qMin(0.0, -width()+parentItem()->width()), x, 0.0));
}

void ContentItem::animateX(qreal newX)
{
    if (!parentItem()) {
        return;
    }

    const qreal to = qBound(qMin(0.0, -width()+parentItem()->width()), newX, 0.0);

    m_slideAnim->setStartValue(x());
    m_slideAnim->setEndValue(to);
    m_slideAnim->start();
}

void ContentItem::snapToItem()
{
    QQuickItem *firstItem = childAt(-x(), 0);
    if (!firstItem) {
        return;
    }
    QQuickItem *nextItem = childAt(firstItem->x() + firstItem->width() + 1, 0);

    //need to make the last item visible?
    if (nextItem && width() - (-x() + m_view->width()) < -x() - firstItem->x()) {
        m_viewAnchorItem = nextItem;
        animateX(-nextItem->x());

    //The first one found?
    } else if (-x() <= firstItem->x() + firstItem->width()/2 || !nextItem) {
        m_viewAnchorItem = firstItem;
        animateX(-firstItem->x());

    //the second?
    } else {
        m_viewAnchorItem = nextItem;
        animateX(-nextItem->x());
    }
}

qreal ContentItem::childWidth(QQuickItem *child)
{
    if (!parentItem()) {
        return 0.0;
    }

    if (m_columnResizeMode == ColumnsView::SingleColumn) {
        return parentItem()->width();

    } else if (m_columnResizeMode == ColumnsView::FixedColumns) {
        ColumnsViewAttached *attached = qobject_cast<ColumnsViewAttached *>(qmlAttachedPropertiesObject<ColumnsView>(child, true));
        if (attached->fillWidth()) {
            return qBound(m_columnWidth, (parentItem()->width() - attached->reservedSpace()), parentItem()->width());
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
    int i = 0;
    for (QQuickItem *child : m_items) {
        if (child->isVisible()) {
            child->setSize(QSizeF(childWidth(child), height()));
            child->setPosition(QPointF(partialWidth, 0.0));
            partialWidth += child->width();
        }
        ColumnsViewAttached *attached = qobject_cast<ColumnsViewAttached *>(qmlAttachedPropertiesObject<ColumnsView>(child, true));
        attached->setLevel(i++);
    }
    setWidth(partialWidth);

    setBoundedX((m_viewAnchorItem ? -m_viewAnchorItem->x() : 0.0));
    setY(0);
    updateVisibleItems();
}

void ContentItem::updateVisibleItems()
{
    QList <QQuickItem *> newItems;

    for (auto *item : m_items) {
        if (item->isVisible() && item->x() + x() < width() && item->x() + item->width() + x() > 0) {
            newItems << item;
        }
    }

    if (newItems != m_visibleItems) {
        m_visibleItems = newItems;
        emit m_view->visibleItemsChanged();
    }
}

void ContentItem::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    switch (change) {
    case QQuickItem::ItemChildAddedChange: {
        ColumnsViewAttached *attached = qobject_cast<ColumnsViewAttached *>(qmlAttachedPropertiesObject<ColumnsView>(value.item, true));
        attached->setView(m_view);

        connect(attached, &ColumnsViewAttached::fillWidthChanged, this, &ContentItem::layoutItems);
        connect(attached, &ColumnsViewAttached::reservedSpaceChanged, this, &ContentItem::layoutItems);

        if (!m_items.contains(value.item)) {
            connect(value.item, &QQuickItem::widthChanged, this, &ContentItem::layoutItems);
            m_items << value.item;
        }
        m_view->polish();
        emit m_view->depthChanged();
        break;
    }
    case QQuickItem::ItemChildRemovedChange: {
        ColumnsViewAttached *attached = qobject_cast<ColumnsViewAttached *>(qmlAttachedPropertiesObject<ColumnsView>(value.item, true));
        attached->setView(nullptr);
        attached->setLevel(-1);

        disconnect(attached, nullptr, this, nullptr);
        disconnect(value.item, nullptr, this, nullptr);

        const int index = m_items.indexOf(value.item);
        m_items.removeAll(value.item);
        m_view->polish();
        if (index < m_view->currentIndex()) {
            m_view->setCurrentIndex(qBound(0, index - 1, m_items.count() - 1));
        }
        emit m_view->depthChanged();
        break;
    }
    case QQuickItem::ItemVisibleHasChanged:
        updateVisibleItems();
        break;
    default:
        break;
    }
    QQuickItem::itemChange(change, value);
}

void ContentItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    updateVisibleItems();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}







ColumnsView::ColumnsView(QQuickItem *parent)
    : QQuickItem(parent),
      m_contentItem(nullptr)
{
    //NOTE: this is to *not* trigger itemChange
    m_contentItem = new ContentItem(this);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildMouseEvents(true);
}

ColumnsView::~ColumnsView()
{
}

ColumnsView::ColumnResizeMode ColumnsView::columnResizeMode() const
{
    return m_contentItem->m_columnResizeMode;
}

void ColumnsView::setColumnResizeMode(ColumnResizeMode mode)
{
    if (m_contentItem->m_columnResizeMode == mode) {
        return;
    }

    m_contentItem->m_columnResizeMode = mode;
    polish();
    emit columnResizeModeChanged();
}

qreal ColumnsView::columnWidth() const
{
    return m_contentItem->m_columnWidth;
}

void ColumnsView::setColumnWidth(qreal width)
{
    if (m_contentItem->m_columnWidth == width) {
        return;
    }

    m_contentItem->m_columnWidth = width;
    polish();
    emit columnWidthChanged();
}

int ColumnsView::currentIndex() const
{
    return m_currentIndex;
}

void ColumnsView::setCurrentIndex(int index)
{
    if (!parentItem() || m_currentIndex == index || index < -1 || index >= m_contentItem->m_items.count()) {
        return;
    }

    m_currentIndex = index;

    if (index == -1) {
        m_currentItem.clear();
    } else {
        m_currentItem = m_contentItem->m_items[index];
        Q_ASSERT(m_currentItem);
        m_currentItem->forceActiveFocus();

        // If the current item is not on view, scroll
        QRectF mapped = m_currentItem->mapRectToItem(parentItem(), QRectF(m_currentItem->position(), m_currentItem->size()));
        if (!QRectF(QPointF(0, 0), parentItem()->size()).intersects(mapped)) {
            m_contentItem->m_viewAnchorItem = m_currentItem;
            m_contentItem->animateX(-m_currentItem->x());
        }
    }

    emit currentIndexChanged();
    emit currentItemChanged();
}

QQuickItem *ColumnsView::currentItem()
{
    return m_currentItem;
}

QList<QQuickItem *>ColumnsView::visibleItems() const
{
    return m_contentItem->m_visibleItems;
}

int ColumnsView::depth() const
{
    return m_contentItem->m_items.count();
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
    m_contentItem->m_viewAnchorItem = item;
    setCurrentIndex(pos);
    item->setParentItem(m_contentItem);
    item->forceActiveFocus();
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
    polish();
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

ColumnsViewAttached *ColumnsView::qmlAttachedProperties(QObject *object)
{
    return new ColumnsViewAttached(object);
}

void ColumnsView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_contentItem->setHeight(newGeometry.height());
    polish();

    m_contentItem->updateVisibleItems();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

bool ColumnsView::childMouseEventFilter(QQuickItem *item, QEvent *event)
{
    if (item == m_contentItem) {
        return QQuickItem::childMouseEventFilter(item, event);
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        m_oldMouseX = m_startMouseX = mapFromItem(item, me->localPos()).x();
        event->accept();
        break;
    }
    case QEvent::MouseMove: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        const QPointF pos = mapFromItem(item, me->localPos());
        m_contentItem->setBoundedX(m_contentItem->x() + pos.x() - m_oldMouseX);
        m_oldMouseX = pos.x();
        event->accept();
        break;
    }
    case QEvent::MouseButtonRelease: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        m_contentItem->snapToItem();
        event->accept();
        return qAbs(mapFromItem(item, me->localPos()).x() - m_startMouseX) > qApp->styleHints()->startDragDistance();
        break;
    }
    default:
        break;
    }

    return QQuickItem::childMouseEventFilter(item, event);
}

void ColumnsView::mousePressEvent(QMouseEvent *event)
{
    m_oldMouseX = event->localPos().x();
    m_startMouseX = event->localPos().x();
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
    m_contentItem->snapToItem();
    event->accept();
}

void ColumnsView::updatePolish()
{
    m_contentItem->layoutItems();
}

void ColumnsView::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    switch (change) {
    case QQuickItem::ItemChildAddedChange:
        if (m_contentItem && value.item != m_contentItem && !value.item->inherits("QQuickRepeater")) {
            addItem(value.item);
        }
        break;
    default:
        break;
    }
    QQuickItem::itemChange(change, value);
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

void ColumnsView::contentData_append(QQmlListProperty<QObject> *prop, QObject *object)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return;
    }

    view->m_contentData.append(object);
    QQuickItem *item = qobject_cast<QQuickItem *>(object);
    //exclude repeaters from layout
    if (item && item->inherits("QQuickRepeater")) {
        item->setParentItem(view);
    } else if (item) {
        view->addItem(item);
    } else {
        object->setParent(view);
    }
}

int ColumnsView::contentData_count(QQmlListProperty<QObject> *prop)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return 0;
    }

    return view->m_contentData.count();
}

QObject *ColumnsView::contentData_at(QQmlListProperty<QObject> *prop, int index)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return nullptr;
    }

    if (index < 0 || index >= view->m_contentData.count()) {
        return nullptr;
    }
    return view->m_contentData.value(index);
}

void ColumnsView::contentData_clear(QQmlListProperty<QObject> *prop)
{
    ColumnsView *view = static_cast<ColumnsView *>(prop->object);
    if (!view) {
        return;
    }

    return view->m_contentData.clear();
}

QQmlListProperty<QObject> ColumnsView::contentData()
{
    return QQmlListProperty<QObject>(this, nullptr,
                                     contentData_append,
                                     contentData_count,
                                     contentData_at,
                                     contentData_clear);
}

#include "moc_columnsview.cpp"
