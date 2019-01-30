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
class ColumnsView;

class ColumnsViewAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(bool fillWidth MEMBER m_fillWidth NOTIFY fillWidthChanged)
    Q_PROPERTY(ColumnsView *view READ view NOTIFY viewChanged)

public:
    ColumnsViewAttached(QObject *parent = nullptr);
    ~ColumnsViewAttached();

    void setLevel(int level);
    int  level() const;

    ColumnsView *view();

Q_SIGNALS:
    void levelChanged();
    void fillWidthChanged();
    void viewChanged();

private:
    int m_level = -1;
    bool m_fillWidth = false;
    
};

class ColumnsView : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(ColumnResizeMode columnResizeMode READ columnResizeMode WRITE setColumnResizeMode NOTIFY columnResizeModeChanged)
    Q_PROPERTY(QQuickItem *stretchableItem READ stretchableItem WRITE setStretchableItem NOTIFY stretchableItemChanged)
    Q_PROPERTY(qreal columnWidth READ columnWidth WRITE setColumnWidth NOTIFY columnWidthChanged)
    Q_PROPERTY(int reservedColumns READ reservedColumns WRITE setReservedColumns NOTIFY reservedColumnsChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QQuickItem *currentItem READ currentItem NOTIFY currentItemChanged)

    Q_PROPERTY(QQuickItem *contentItem READ contentItem CONSTANT)

    Q_PROPERTY(QList<QQuickItem *> visibleItems READ visibleItems NOTIFY visibleItemsChanged)

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
    ColumnsView(QQuickItem *parent = nullptr);
    ~ColumnsView();

    ColumnResizeMode columnResizeMode() const;
    void setColumnResizeMode(ColumnResizeMode mode);

    QQuickItem *stretchableItem() const;
    void setStretchableItem(QQuickItem *item);

    qreal columnWidth() const;
    void setColumnWidth(qreal width);

    int reservedColumns() const;
    void setReservedColumns(int columns);

    int currentIndex() const;
    void setCurrentIndex(int index);

    QQuickItem *currentItem();

    QList<QQuickItem *>visibleItems() const;


    QQuickItem *contentItem() const;

    QQmlListProperty<QQuickItem> contentChildren();
    QQmlListProperty<QObject> contentData();

    //can't do overloads in QML
    void removeItem(QQuickItem *item);
    void removeItem(int item);

    //QML attached property
    static ColumnsViewAttached *qmlAttachedProperties(QObject *object);

public Q_SLOTS:
    void addItem(QQuickItem *item);
    void insertItem(int pos, QQuickItem *item);
    void moveItem(int from, int to);
    void removeItem(const QVariant &item);
    void clear();

protected:
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void contentChildrenChanged();
    void columnResizeModeChanged();
    void stretchableItemChanged();
    void columnWidthChanged();
    void reservedColumnsChanged();
    void currentIndexChanged();
    void currentItemChanged();
    void visibleItemsChanged();

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

    static QHash<QObject *, ColumnsViewAttached *> m_attachedObjects;
    qreal m_oldMouseX;
    int m_currentIndex = -1;
};

QML_DECLARE_TYPEINFO(ColumnsView, QML_HAS_ATTACHED_PROPERTIES)
