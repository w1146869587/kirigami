#pragma once

#include <QQuickItem>
#include "columnview.h"

struct ParsedRoute {
    QString name;
    QVariant data;
};

class PageRouterAttached;

class PageRouter : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QJSValue routes READ routes WRITE setRoutes NOTIFY routesChanged)
    Q_PROPERTY(QJSValue initialRoute READ initialRoute WRITE setInitialRoute NOTIFY initialRouteChanged)
    Q_PROPERTY(QQuickItem* pageRow MEMBER m_pageRow)

private:
    QJSValue m_routes;
    QQuickItem* m_pageRow;
    QJSValue m_initialRoute;
    QList<ParsedRoute> m_currentRoutes;

    void push(ParsedRoute route);
    bool routesContainsKey(const QString &key);
    QQmlComponent *routesValueForKey(const QString &key);

protected:
    void classBegin() override;
    void componentComplete() override;

public:
    PageRouter(QQuickItem *parent = nullptr);
    ~PageRouter();

    QJSValue routes() const;
    void setRoutes(QJSValue routes);

    QJSValue initialRoute() const;
    void setInitialRoute(QJSValue initialRoute);

    Q_INVOKABLE void navigateToRoute(QJSValue route);
    Q_INVOKABLE bool isNavigatedToRoute(QJSValue route);
    Q_INVOKABLE void pushRoute(QJSValue route);
    Q_INVOKABLE void popRoute();

    static PageRouterAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void routesChanged();
    void initialRouteChanged();
};

class PageRouterAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PageRouter *router READ router NOTIFY routerChanged)
    Q_PROPERTY(QVariant data MEMBER m_data NOTIFY dataChanged)

private:
    explicit PageRouterAttached(QObject *parent = nullptr);

    QPointer<PageRouter> m_router;
    QVariant m_data;

    friend class PageRouter;

public:
    PageRouter* router() const { return m_router; };
    QVariant data() const { return m_data; };
    Q_INVOKABLE void navigateToRoute(QJSValue route) { m_router->navigateToRoute(route); };
    Q_INVOKABLE bool isNavigatedToRoute(QJSValue route) { return m_router->isNavigatedToRoute(route); };
    Q_INVOKABLE void pushRoute(QJSValue route) { m_router->pushRoute(route); };
    Q_INVOKABLE void popRoute() { m_router->popRoute(); };

Q_SIGNALS:
    void routerChanged();
    void dataChanged();
};

QML_DECLARE_TYPEINFO(PageRouter, QML_HAS_ATTACHED_PROPERTIES)