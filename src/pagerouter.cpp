#include "pagerouter.h"

ParsedRoute parseRoute(QJSValue value)
{
    if (value.isUndefined()) {
        return ParsedRoute{QString(), QVariant()};
    } else if (value.isString()) {
        return ParsedRoute{
            value.toString(),
            QVariant()
        };
    } else {
        return ParsedRoute{
            value.property(QStringLiteral("route")).toString(),
            value.property(QStringLiteral("data")).toVariant()
        };
    }
}

QList<ParsedRoute> parseRoutes(QList<QJSValue> values)
{
    QList<ParsedRoute> ret;
    for (auto route : values)
        ret << parseRoute(route);
    return ret;
}

PageRouter::PageRouter(QQuickItem *parent) : QQuickItem(parent)
{

}

PageRouter::~PageRouter() {}

void PageRouter::classBegin()
{
    QQmlComponent *component = new QQmlComponent(qmlEngine(this), this);
    component->setData(
        QByteArrayLiteral("import QtQuick 2.0; import org.kde.Kirigami 2.7 as Kirigami; PageRow {}"),
        QUrl(QStringLiteral("pagerouter.cpp"))
    );
    m_pageRow = static_cast<QQuickItem*>(component->create(qmlContext(this)));
    connect(this, &PageRouter::initialRouteChanged,
            [=]() {
                QMetaObject::invokeMethod(m_pageRow, "clear");
                push(parseRoute(initialRoute()));
            });
}

void PageRouter::push(ParsedRoute route)
{
    if (!m_routes.keys().contains(route.name)) {
        qCritical() << "Route" << route.name << "not defined";
        return;
    }
    auto context = qmlContext(this);
    auto component = m_routes.value(route.name);
    if (component->status() == QQmlComponent::Ready) {
        auto item = component->create(context);
        // TODO: pester mart about seeing if he knows
        // how to utilise an attached property instead
        item->setProperty("routeData", route.data);
        QMetaObject::invokeMethod(m_pageRow, "push", Q_ARG(QObject*, item));
        m_currentRoutes << route;
    } else if (component->status() == QQmlComponent::Loading) {
        connect(component, &QQmlComponent::statusChanged, [=](QQmlComponent::Status status) {
            if (status != QQmlComponent::Ready) {
                qCritical() << "Failed to push route:" << component->errors();
            }
            auto item = component->create(context);
            // TODO: See above
            item->setProperty("routeData", route.data);
            QMetaObject::invokeMethod(m_pageRow, "push", Q_ARG(QObject*, item));
            m_currentRoutes << route;
        });
    } else {
        qCritical() << "Failed to push route:" << component->errors();
    }
}

ComponentMap PageRouter::routes() const
{
    return m_routes;
}

void PageRouter::setRoutes(const ComponentMap &routes)
{
    m_routes = routes;
}

QJSValue PageRouter::initialRoute() const
{
    return m_initialRoute;
}

void PageRouter::setInitialRoute(QJSValue value)
{
    m_initialRoute = value;
}

void PageRouter::navigateToRoute(QList<QJSValue> route)
{
    QMetaObject::invokeMethod(m_pageRow, "clear");
    for (auto route : parseRoutes(route)) {
        push(route);
    }
}

bool PageRouter::isNavigatedToRoute(QList<QJSValue> route)
{
    auto parsed = parseRoutes(route);
    if (parsed.length() > m_currentRoutes.length()) {
        return false;
    }
    for (int i = 0; i < parsed.length(); i++) {
        if (parsed[i].name != m_currentRoutes[i].name || parsed[i].data != m_currentRoutes[i].data) {
            return false;
        }
    }
    return true;
}

void PageRouter::pushRoute(QJSValue route)
{
    push(parseRoute(route));
}

void PageRouter::popRoute()
{
    QMetaObject::invokeMethod(m_pageRow, "pop");
}