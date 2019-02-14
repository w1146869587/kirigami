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
class ColumnView;

class ColumnViewAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(bool fillWidth READ fillWidth WRITE setFillWidth NOTIFY fillWidthChanged)
    Q_PROPERTY(qreal reservedSpace READ reservedSpace WRITE setReservedSpace NOTIFY reservedSpaceChanged)
    Q_PROPERTY(ColumnView *view READ view NOTIFY viewChanged)

public:
    ColumnViewAttached(QObject *parent = nullptr);
    ~ColumnViewAttached();

    void setLevel(int level);
    int  level() const;

    void setFillWidth(bool fill);
    bool fillWidth() const;

    qreal reservedSpace() const;
    void setReservedSpace(qreal space);

    ColumnView *view();
    void setView(ColumnView *view);

Q_SIGNALS:
    void levelChanged();
    void fillWidthChanged();
    void reservedSpaceChanged();
    void viewChanged();

private:
    int m_level = -1;
    bool m_fillWidth = false;
    qreal m_reservedSpace = 0;
    QPointer<ColumnView> m_view;
    bool m_customFillWidth = false;
    bool m_customReservedSpace = false;
};

class ColumnView : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(ColumnResizeMode columnResizeMode READ columnResizeMode WRITE setColumnResizeMode NOTIFY columnResizeModeChanged)
    Q_PROPERTY(qreal columnWidth READ columnWidth WRITE setColumnWidth NOTIFY columnWidthChanged)
    Q_PROPERTY(int depth READ depth NOTIFY depthChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QQuickItem *currentItem READ currentItem NOTIFY currentItemChanged)

    Q_PROPERTY(QQuickItem *contentItem READ contentItem CONSTANT)
    Q_PROPERTY(qreal contentX READ contentX WRITE setContentX NOTIFY contentXChanged)
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(int scrollDuration READ scrollDuration WRITE setScrollDuration NOTIFY scrollDurationChanged)
    Q_PROPERTY(bool separatorVisible READ separatorVisible WRITE setSeparatorVisible NOTIFY separatorVisibleChanged)

    Q_PROPERTY(QList<QObject *> visibleItems READ visibleItems NOTIFY visibleItemsChanged)

    // Properties to make it similar to Flickable
    Q_PROPERTY(bool dragging READ dragging NOTIFY draggingChanged)
    Q_PROPERTY(bool moving READ moving NOTIFY movingChanged)
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)

    // Default properties
    Q_PROPERTY(QQmlListProperty<QQuickItem> contentChildren READ contentChildren NOTIFY contentChildrenChanged FINAL)
    Q_PROPERTY(QQmlListProperty<QObject> contentData READ contentData  FINAL)
    Q_CLASSINFO("DefaultProperty", "contentData")

    Q_ENUMS(ColumnResizeMode)

public:
    enum ColumnResizeMode {
        FixedColumns = 0,
        DynamicColumns,
        SingleColumn
    };
    ColumnView(QQuickItem *parent = nullptr);
    ~ColumnView();

    // QML property accessors
    ColumnResizeMode columnResizeMode() const;
    void setColumnResizeMode(ColumnResizeMode mode);

    qreal columnWidth() const;
    void setColumnWidth(qreal width);

    int currentIndex() const;
    void setCurrentIndex(int index);

    int scrollDuration() const;
    void setScrollDuration(int duration);

    bool separatorVisible() const;
    void setSeparatorVisible(bool visible);

    int depth() const;

    QQuickItem *currentItem();

    //NOTE: It's a QList<QObject *> as QML can't corectly build an Array out of QList<QQuickItem*>
    QList<QObject *> visibleItems() const;


    QQuickItem *contentItem() const;

    QQmlListProperty<QQuickItem> contentChildren();
    QQmlListProperty<QObject> contentData();

    bool dragging() const;
    bool moving() const;
    qreal contentWidth() const;

    qreal contentX() const;
    void setContentX(qreal x) const;

    bool interactive() const;
    void setInteractive(bool interactive);

    // Api not intended for QML use
    //can't do overloads in QML
    void removeItem(QQuickItem *item);
    void removeItem(int item);

    // QML attached property
    static ColumnViewAttached *qmlAttachedProperties(QObject *object);

public Q_SLOTS:
    void addItem(QQuickItem *item);
    void insertItem(int pos, QQuickItem *item);
    void moveItem(int from, int to);
    void removeItem(const QVariant &item);
    void pop(QQuickItem *item);
    void clear();
    bool containsItem(QQuickItem *item);

protected:
    void classBegin() override;
    void updatePolish() override;
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    bool childMouseEventFilter(QQuickItem *item, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseUngrabEvent() override;

Q_SIGNALS:
    void contentChildrenChanged();
    void columnResizeModeChanged();
    void columnWidthChanged();
    void currentIndexChanged();
    void currentItemChanged();
    void visibleItemsChanged();
    void depthChanged();
    void draggingChanged();
    void movingChanged();
    void contentXChanged();
    void contentWidthChanged();
    void interactiveChanged();
    void scrollDurationChanged();
    void separatorVisibleChanged();

private:
    static void contentChildren_append(QQmlListProperty<QQuickItem> *prop, QQuickItem *object);
    static int contentChildren_count(QQmlListProperty<QQuickItem> *prop);
    static QQuickItem *contentChildren_at(QQmlListProperty<QQuickItem> *prop, int index);
    static void contentChildren_clear(QQmlListProperty<QQuickItem> *prop);

    static void contentData_append(QQmlListProperty<QObject> *prop, QObject *object);
    static int contentData_count(QQmlListProperty<QObject> *prop);
    static QObject *contentData_at(QQmlListProperty<QObject> *prop, int index);
    static void contentData_clear(QQmlListProperty<QObject> *prop);


    QList<QObject *> m_contentData;

    ContentItem *m_contentItem;
    QPointer<QQuickItem> m_currentItem;

    static QHash<QObject *, ColumnViewAttached *> m_attachedObjects;
    qreal m_oldMouseX = -1.0;
    qreal m_startMouseX = -1.0;
    int m_currentIndex = -1;

    bool m_interactive = true;
    bool m_dragging = false;
    bool m_moving = false;
    bool m_separatorVisible = true;
};

QML_DECLARE_TYPEINFO(ColumnView, QML_HAS_ATTACHED_PROPERTIES)
