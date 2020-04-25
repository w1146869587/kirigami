#include <QDebug>
#include <cmath>
#include "physics_p.h"

DragSimulation::DragSimulation(QObject *parent) : QObject(parent), m_tolerance(new Tolerance) {}

auto DragSimulation::xAtTime(double time) -> double
{
    if (m_friction == 0) {
        return (m_start)+(m_velocity*time);
    }
    return (m_start) +
           (xVelocityAtTime(time) / log(m_friction)) -
           (m_velocity / log(m_friction));
}

auto DragSimulation::xVelocityAtTime(double time) -> double
{
    return m_velocity * pow(m_friction, time);
}

auto DragSimulation::simulationWithinTolerance(double time) -> bool
{
    return abs(xVelocityAtTime(time)) < m_tolerance->velocity;
}