#include <QQmlProperty>
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

QList<ParsedRoute> parseRoutes(QJSValue values)
{
    QList<ParsedRoute> ret;
    if (values.isArray()) {
        for (auto route : values.toVariant().toList()) {
            if (route.toString() != QString()) {
                ret << ParsedRoute{
                    route.toString(),
                    QVariant()
                };
            } else if (route.canConvert<QVariantMap>()) {
                auto map = route.value<QVariantMap>();
                ret << ParsedRoute{
                    map.value(QStringLiteral("route")).toString(),
                    map.value(QStringLiteral("data"))
                };
            }
        }
    } else {
        ret << parseRoute(values);
    }
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
        QByteArrayLiteral("import QtQuick 2.0; import org.kde.kirigami 2.7 as Kirigami; Kirigami.PageRow { anchors.fill: parent }"),
        QUrl(QStringLiteral("pagerouter.cpp"))
    );
    m_pageRow = static_cast<QQuickItem*>(component->create(qmlContext(this)));
    QQmlProperty::write(m_pageRow, QStringLiteral("parent"), QVariant::fromValue(this));
    connect(this, &PageRouter::initialRouteChanged,
            [=]() {
                QMetaObject::invokeMethod(m_pageRow, "clear");
                m_currentRoutes.clear();
                push(parseRoute(initialRoute()));
            });
}

void PageRouter::componentComplete()
{
    QMetaObject::invokeMethod(m_pageRow, "clear");
    m_currentRoutes.clear();
    push(parseRoute(initialRoute()));
}

bool PageRouter::routesContainsKey(const QString &key)
{
    return m_routes.hasProperty(key);
}

QQmlComponent* PageRouter::routesValueForKey(const QString &key)
{
    return m_routes.property(key).toVariant().value<QQmlComponent*>();
}

void PageRouter::push(ParsedRoute route)
{
    if (!routesContainsKey(route.name)) {
        qCritical() << "Route" << route.name << "not defined";
        return;
    }
    auto context = qmlContext(this);
    auto component = routesValueForKey(route.name);
    if (component->status() == QQmlComponent::Ready) {
        auto item = component->beginCreate(context);
        item->setParent(this);
        auto clone = route;
        clone.item = item;
        m_currentRoutes << clone;
        component->completeCreate();
        QMetaObject::invokeMethod(m_pageRow, "push", Q_ARG(QVariant, QVariant::fromValue(item)), Q_ARG(QVariant, QVariant()));
    } else if (component->status() == QQmlComponent::Loading) {
        connect(component, &QQmlComponent::statusChanged, [=](QQmlComponent::Status status) {
            if (status != QQmlComponent::Ready) {
                qCritical() << "Failed to push route:" << component->errors();
            }
            auto item = component->beginCreate(context);
            item->setParent(this);
            auto clone = route;
            clone.item = item;
            m_currentRoutes << clone;
            component->completeCreate();
            QMetaObject::invokeMethod(m_pageRow, "push", Q_ARG(QVariant, QVariant::fromValue(item)), Q_ARG(QVariant, QVariant()));
        });
    } else {
        qCritical() << "Failed to push route:" << component->errors();
    }
}

QJSValue PageRouter::routes() const
{
    return m_routes;
}

void PageRouter::setRoutes(QJSValue routes)
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

void PageRouter::navigateToRoute(QJSValue route)
{
    QMetaObject::invokeMethod(m_pageRow, "clear");
    m_currentRoutes.clear();
    for (auto route : parseRoutes(route)) {
        push(route);
    }
}

bool PageRouter::isNavigatedToRoute(QJSValue route)
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

QVariant PageRouter::dataFor(QObject *object)
{
    auto pointer = object;
    while (pointer != nullptr) {
        for (auto route : m_currentRoutes) {
            if (route.item == pointer) {
                return route.data;
            }
        }
        pointer = pointer->parent();
    }
    return QVariant();
}

PageRouterAttached* PageRouter::qmlAttachedProperties(QObject *object)
{
    auto attached = new PageRouterAttached(object);
    auto pointer = object;
    while (pointer != nullptr) {
        auto casted = qobject_cast<PageRouter*>(pointer);
        if (casted != nullptr) {
            attached->m_router = casted;
            break;
        }
        pointer = pointer->parent();
    }
    if (attached->m_router.isNull()) {
        qCritical() << "PageRouterAttached could not find a parent PageRouter";
    }
    return attached;
}

QVariant PageRouterAttached::data() const
{
    if (m_router) {
        return m_router->dataFor(parent());
    } else {
        qCritical() << "PageRouterAttached does not have a parent PageRouter";
        return QVariant();
    }
}

PageRouterAttached::PageRouterAttached(QObject *parent) : QObject(parent) {}