#pragma once
#include <QObject>

#define DEFAULT_TOLERANCE 1e-3

struct Tolerance : public QObject {
    Q_OBJECT

    Q_PROPERTY(double distance MEMBER distance)
    Q_PROPERTY(double time MEMBER time)
    Q_PROPERTY(double velocity MEMBER velocity)

public:
    double distance = DEFAULT_TOLERANCE;
    double time = DEFAULT_TOLERANCE;
    double velocity = DEFAULT_TOLERANCE;

    bool operator==(Tolerance rhs) {
        return distance == rhs.distance && time == rhs.time && velocity == rhs.velocity;
    };
    bool operator!=(Tolerance rhs) {
        return distance != rhs.distance || time != rhs.time || velocity != rhs.velocity;
    };
};

class Simulation {

public:
    virtual double xAtTime(double time) = 0;
    virtual double xVelocityAtTime(double time) = 0;
    virtual bool simulationWithinTolerance(double time) = 0;
};

class DragSimulation : public QObject, Simulation {
    Q_OBJECT

    Q_PROPERTY(Tolerance* tolerance MEMBER m_tolerance)
    Q_PROPERTY(double friction MEMBER m_friction)
    Q_PROPERTY(double start MEMBER m_start)
    Q_PROPERTY(double velocity MEMBER m_velocity)

private:
    Tolerance* m_tolerance;
    double m_friction;
    double m_start;
    double m_velocity;

public:
    DragSimulation(QObject *parent = nullptr);
    ~DragSimulation() {};

    Q_INVOKABLE double xAtTime(double time) override;
    Q_INVOKABLE double xVelocityAtTime(double time) override;
    Q_INVOKABLE bool simulationWithinTolerance(double time) override;
};