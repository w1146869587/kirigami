#pragma once

#include <QQuickItem>
#include "columnview.h"

typedef QMap<QString, QQmlComponent*> ComponentMap;

Q_DECLARE_METATYPE(ComponentMap)

struct ParsedRoute {
    QString name;
    QVariant data;
};

class PageRouter : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(ComponentMap routes READ routes WRITE setRoutes NOTIFY routesChanged)
    Q_PROPERTY(QJSValue initialRoute READ initialRoute WRITE setInitialRoute NOTIFY initialRouteChanged)

private:
    ComponentMap m_routes;
    QQuickItem* m_pageRow;
    QJSValue m_initialRoute;
    QList<ParsedRoute> m_currentRoutes;

    void push(ParsedRoute route);

protected:
    void classBegin() override;

public:
    PageRouter(QQuickItem *parent = nullptr);
    ~PageRouter();

    ComponentMap routes() const;
    void setRoutes(const ComponentMap &routes);

    QJSValue initialRoute() const;
    void setInitialRoute(QJSValue initialRoute);

    Q_INVOKABLE void navigateToRoute(QList<QJSValue> route);
    Q_INVOKABLE bool isNavigatedToRoute(QList<QJSValue> route);
    Q_INVOKABLE void pushRoute(QJSValue route);
    Q_INVOKABLE void popRoute();

Q_SIGNALS:
    void routesChanged();
    void initialRouteChanged();
};
