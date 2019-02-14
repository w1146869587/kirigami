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

class QmlComponentsPoolSingleton
{
public:
    QmlComponentsPoolSingleton()
    {}

    QmlComponentsPool self;
};

Q_GLOBAL_STATIC(QmlComponentsPoolSingleton, privateQmlComponentsPoolSelf)


QmlComponentsPool::QmlComponentsPool(QObject *parent)
    : QObject(parent)
{}

void QmlComponentsPool::initialize(QQmlEngine *engine)
{
    if (!engine || m_instance) {
        return;
    }

    QQmlComponent *component = new QQmlComponent(engine, this);

    component->setData(QByteArrayLiteral("import QtQuick 2.7\n"
        "import org.kde.kirigami 2.7 as Kirigami\n"
        "QtObject {\n"
            "id: root\n"
            "readonly property Kirigami.Units units: Kirigami.Units\n"
            "readonly property Component separator: Kirigami.Separator {"
                "property Item column\n"
                "visible: column.Kirigami.ColumnsView.view.contentX < column.x;"
                "anchors.top: column.top;"
                "anchors.bottom: column.bottom;"
            "}"
        "}"), QUrl());

    m_instance = component->create();
    //qWarning()<<component->errors();
    Q_ASSERT(m_instance);

    m_separatorComponent = m_instance->property("separator").value<QQmlComponent *>();
    Q_ASSERT(m_separatorComponent);

    m_units = m_instance->property("units").value<QObject *>();
    Q_ASSERT(m_units);

    connect(m_units, SIGNAL(gridUnitChanged()), this, SIGNAL(gridUnitChanged()));
    connect(m_units, SIGNAL(longDurationChanged()), this, SIGNAL(longDurationChanged()));
}

QmlComponentsPool::~QmlComponentsPool()
{}


/////////

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
    //NOTE: the duration will be taked from kirigami units upon classBegin
    m_slideAnim->setDuration(0);
    m_slideAnim->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    connect(m_slideAnim, &QPropertyAnimation::finished, this, [this] () {
        if (!m_view->currentItem()) {
            m_view->setCurrentIndex(m_items.indexOf(m_viewAnchorItem));
        } else {
            QRectF mapped = m_view->currentItem()->mapRectToItem(parentItem(), QRectF(m_view->currentItem()->position(), m_view->currentItem()->size()));
            if (!QRectF(QPointF(0, 0), size()).intersects(mapped)) {
                m_view->setCurrentIndex(m_items.indexOf(m_viewAnchorItem));
            }
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
    setX(qRound(qBound(qMin(0.0, -width()+parentItem()->width()), x, 0.0)));
}

void ContentItem::animateX(qreal newX)
{
    if (!parentItem()) {
        return;
    }

    const qreal to = qRound(qBound(qMin(0.0, -width()+parentItem()->width()), newX, 0.0));

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

    ColumnsViewAttached *attached = qobject_cast<ColumnsViewAttached *>(qmlAttachedPropertiesObject<ColumnsView>(child, true));

    if (m_columnResizeMode == ColumnsView::SingleColumn) {
        return qRound(parentItem()->width());

    } else if (attached->fillWidth()) {
        return qRound(qBound(m_columnWidth, (parentItem()->width() - attached->reservedSpace()), parentItem()->width()));

    } else if (m_columnResizeMode == ColumnsView::FixedColumns) {
        return qRound(qMin(parentItem()->width(), m_columnWidth));

    // DynamicColumns
    } else {
        //TODO:look for Layout size hints
        qreal width = child->implicitWidth();

        if (width < 1.0) {
            width = m_columnWidth;
        }

        return qRound(qMin(m_view->width(), width));
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

    const qreal newContentX = m_viewAnchorItem ? -m_viewAnchorItem->x() : 0.0;
    if (m_shouldAnimate) {
        animateX(newContentX);
    } else {
        setBoundedX(newContentX);
    }
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

void ContentItem::forgetItem(QQuickItem *item)
{
    if (!m_items.contains(item)) {
        return;
    }

    ColumnsViewAttached *attached = qobject_cast<ColumnsViewAttached *>(qmlAttachedPropertiesObject<ColumnsView>(item, true));
    attached->setView(nullptr);
    attached->setLevel(-1);

    disconnect(attached, nullptr, this, nullptr);
    disconnect(item, nullptr, this, nullptr);

    QQuickItem *separatorItem = m_separators.take(item);
    if (separatorItem) {
        separatorItem->deleteLater();
    }

    const int index = m_items.indexOf(item);
    m_items.removeAll(item);
    m_shouldAnimate = true;
    m_view->polish();

    if (index <= m_view->currentIndex()) {
        m_view->setCurrentIndex(qBound(0, index - 1, m_items.count() - 1));
    }
    emit m_view->depthChanged();
}

QQuickItem *ContentItem::ensureSeparator(QQuickItem *item)
{
    QQuickItem *separatorItem = m_separators.value(item);
    
    if (!separatorItem) {
        separatorItem = qobject_cast<QQuickItem *>(privateQmlComponentsPoolSelf->self.m_separatorComponent->beginCreate(QQmlEngine::contextForObject(item)));
        if (separatorItem) {
            separatorItem->setParentItem(item);
            separatorItem->setZ(9999);
            separatorItem->setProperty("column", QVariant::fromValue(item));
            privateQmlComponentsPoolSelf->self.m_separatorComponent->completeCreate();
            m_separators[item] = separatorItem;
        }
    }

    return separatorItem;
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

        if (m_view->separatorsVisible()) {
            ensureSeparator(value.item);
        }

        m_shouldAnimate = true;
        m_view->polish();
        emit m_view->depthChanged();
        break;
    }
    case QQuickItem::ItemChildRemovedChange: {
        forgetItem(value.item);
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

    connect(m_contentItem->m_slideAnim, &QPropertyAnimation::finished, this, [this] () {
        m_moving = false;
        emit movingChanged();
    });
    connect(m_contentItem, &ContentItem::widthChanged, this, &ColumnsView::contentWidthChanged);
    connect(m_contentItem, &ContentItem::xChanged, this, &ColumnsView::contentXChanged);
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
    if (mode == SingleColumn && m_currentItem) {
        m_contentItem->m_viewAnchorItem = m_currentItem;
    }
    m_contentItem->m_shouldAnimate = false;
    polish();
    emit columnResizeModeChanged();
}

qreal ColumnsView::columnWidth() const
{
    return m_contentItem->m_columnWidth;
}

void ColumnsView::setColumnWidth(qreal width)
{
    // Always forget the internal binding when the user sets anything, even the same value
    disconnect(&privateQmlComponentsPoolSelf->self, &QmlComponentsPool::gridUnitChanged, this, nullptr);

    if (m_contentItem->m_columnWidth == width) {
        return;
    }

    m_contentItem->m_columnWidth = width;
    m_contentItem->m_shouldAnimate = false;
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

int ColumnsView::scrollDuration() const
{
    return m_contentItem->m_slideAnim->duration();
}

void ColumnsView::setScrollDuration(int duration)
{
    disconnect(&privateQmlComponentsPoolSelf->self, &QmlComponentsPool::longDurationChanged, this, nullptr);

    if (m_contentItem->m_slideAnim->duration() == duration) {
        return;
    }

    m_contentItem->m_slideAnim->setDuration(duration);
    emit scrollDurationChanged();
}

bool ColumnsView::separatorsVisible() const
{
    return m_separatorsVisible;
}

void ColumnsView::setSeparatorsVisible(bool visible)
{
    if (visible == m_separatorsVisible) {
        return;
    }

    m_separatorsVisible = visible;

    if (visible) {
        for (QQuickItem *item : m_contentItem->m_items) {
            QQuickItem *sep = m_contentItem->ensureSeparator(item);
            if (sep) {
                sep->setVisible(true);
            }
        }
    } else {
        for (QQuickItem *sep : m_contentItem->m_separators.values()) {
            sep->setVisible(false);
        }
    }

    emit separatorsVisibleChanged();
}

bool ColumnsView::dragging() const
{
    return m_dragging;
}

bool ColumnsView::moving() const
{
    return m_moving;
}

qreal ColumnsView::contentWidth() const
{
    return m_contentItem->width();
}

qreal ColumnsView::contentX() const
{
    return -m_contentItem->x();
}

void ColumnsView::setContentX(qreal x) const
{
    m_contentItem->setX(qRound(-x));
}

bool ColumnsView::interactive() const
{
    return m_interactive;
}

void ColumnsView::setInteractive(bool interactive)
{
    if (m_interactive != interactive) {
        return;
    }

    m_interactive = interactive;

    if (!m_interactive) {
        if (m_dragging) {
            m_dragging = false;
            emit draggingChanged();
        }

        m_contentItem->snapToItem();
        setKeepMouseGrab(false);
    }

    emit interactiveChanged();
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
    m_contentItem->m_shouldAnimate = true;
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

    m_contentItem->forgetItem(item);

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

void ColumnsView::pop(QQuickItem *item)
{
    while (!m_contentItem->m_items.isEmpty() && m_contentItem->m_items.last() != item) {
        removeItem(m_contentItem->m_items.last());
        // if no item has been passed, just pop one
        if (!item) {
            break;
        }
    }
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

bool ColumnsView::contains(QQuickItem *item)
{
    return m_contentItem->m_items.contains(item);
}

ColumnsViewAttached *ColumnsView::qmlAttachedProperties(QObject *object)
{
    return new ColumnsViewAttached(object);
}

void ColumnsView::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_contentItem->setHeight(newGeometry.height());
    m_contentItem->m_shouldAnimate = false;
    polish();

    m_contentItem->updateVisibleItems();
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

bool ColumnsView::childMouseEventFilter(QQuickItem *item, QEvent *event)
{
    if (!m_interactive || item == m_contentItem) {
        return QQuickItem::childMouseEventFilter(item, event);
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        m_contentItem->m_slideAnim->stop();
        if (item->property("preventStealing").toBool()) {
            m_contentItem->snapToItem();
            return false;
        }
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        m_oldMouseX = m_startMouseX = mapFromItem(item, me->localPos()).x();

        me->setAccepted(false);
        setKeepMouseGrab(false);

        // On press, we set the current index of the view to the root item 
        QQuickItem *candidateItem = item;
        while (candidateItem->parentItem() && candidateItem->parentItem() != m_contentItem) {
            candidateItem = candidateItem->parentItem();
        }
        if (candidateItem->parentItem() == m_contentItem) {
            setCurrentIndex(m_contentItem->m_items.indexOf(candidateItem));
        }
        break;
    }
    case QEvent::MouseMove: {
        if ((!keepMouseGrab() && item->keepMouseGrab()) || item->property("preventStealing").toBool()) {
            m_contentItem->snapToItem();
            return false;
        }
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        const QPointF pos = mapFromItem(item, me->localPos());

        const bool wasDragging = m_dragging;
        // If a drag happened, start to steal all events, use startDragDistance * 2 to give time to widgets to take the mouse grab by themselves
        m_dragging = keepMouseGrab() || qAbs(mapFromItem(item, me->localPos()).x() - m_startMouseX) > qApp->styleHints()->startDragDistance() * 2;

        if (m_dragging != wasDragging) {
            m_moving = true;
            emit movingChanged();
            emit draggingChanged();
        }

        if (m_dragging) {
            m_contentItem->setBoundedX(m_contentItem->x() + pos.x() - m_oldMouseX);
        }

        m_oldMouseX = pos.x();

        setKeepMouseGrab(m_dragging);
        me->setAccepted(m_dragging);
        return m_dragging;
        break;
    }
    case QEvent::MouseButtonRelease: {
        m_contentItem->snapToItem();
        if (m_dragging) {
            m_dragging = false;
            emit draggingChanged();
        }

        if (item->property("preventStealing").toBool()) {
            return false;
        }
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        event->accept();

        //if a drag happened, don't pass the event
        const bool block = keepMouseGrab();
        setKeepMouseGrab(false);

        me->setAccepted(block);
        return block;
        break;
    }
    default:
        break;
    }

    return QQuickItem::childMouseEventFilter(item, event);
}

void ColumnsView::mousePressEvent(QMouseEvent *event)
{
    if (!m_interactive) {
        return;
    }

    m_contentItem->snapToItem();
    m_oldMouseX = event->localPos().x();
    m_startMouseX = event->localPos().x();
    setKeepMouseGrab(false);
    event->accept();
}

void ColumnsView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_interactive) {
        return;
    }

    const bool wasDragging = m_dragging;
    // Same startDragDistance * 2 as the event filter
    m_dragging = keepMouseGrab() || qAbs(event->localPos().x() - m_startMouseX) > qApp->styleHints()->startDragDistance() * 2;
    if (m_dragging != wasDragging) {
        m_moving = true;
        emit movingChanged();
        emit draggingChanged();
    }

    setKeepMouseGrab(m_dragging);

    if (m_dragging) {
        m_contentItem->setBoundedX(m_contentItem->x() + event->pos().x() - m_oldMouseX);
    }

    m_oldMouseX = event->pos().x();
    event->accept();
}

void ColumnsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_interactive) {
        return;
    }

    if (m_dragging) {
        m_dragging = false;
        emit draggingChanged();
    }

    m_contentItem->snapToItem();
    setKeepMouseGrab(false);
    event->accept();
}

void ColumnsView::mouseUngrabEvent()
{
    if (m_dragging) {
        m_dragging = false;
        emit draggingChanged();
    }

    m_contentItem->snapToItem();
    setKeepMouseGrab(false);
}

void ColumnsView::classBegin()
{
    privateQmlComponentsPoolSelf->self.initialize(qmlEngine(this));

    auto syncColumnWidth = [this]() {
        m_contentItem->m_columnWidth = privateQmlComponentsPoolSelf->self.m_units->property("gridUnit").toInt() * 20;
        emit columnWidthChanged();
    };

    connect(&privateQmlComponentsPoolSelf->self, &QmlComponentsPool::gridUnitChanged, this, syncColumnWidth);
    syncColumnWidth();

    auto syncDuration = [this]() {
        m_contentItem->m_slideAnim->setDuration(privateQmlComponentsPoolSelf->self.m_units->property("longDuration").toInt());
        emit scrollDurationChanged();
    };

    connect(&privateQmlComponentsPoolSelf->self, &QmlComponentsPool::longDurationChanged, this, syncDuration);
    syncDuration();
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
