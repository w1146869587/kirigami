#pragma once

#include <QQuickItem>
#include "columnview.h"

struct ParsedRoute {
    QString name;
    QVariant data;
};

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
    void setRoutes(const QJSValue &routes);

    QJSValue initialRoute() const;
    void setInitialRoute(QJSValue initialRoute);

    Q_INVOKABLE void navigateToRoute(QJSValue route);
    Q_INVOKABLE bool isNavigatedToRoute(QJSValue route);
    Q_INVOKABLE void pushRoute(QJSValue route);
    Q_INVOKABLE void popRoute();

Q_SIGNALS:
    void routesChanged();
    void initialRouteChanged();
};
