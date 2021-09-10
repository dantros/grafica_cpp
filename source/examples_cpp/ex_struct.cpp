
#include <iostream>
#include <cmath>
#include <numbers>

struct Point
{
    float x, y;
};

struct PolarPoint
{
    float magnitude;
    float angle;
};

// Functor or Function Object
struct PointExchanger
{
    Point operator()(const PolarPoint& polarPoint) const
    {
        return {
            polarPoint.magnitude * std::sinf(polarPoint.angle),
            polarPoint.magnitude * std::cosf(polarPoint.angle)
        };
    }

    PolarPoint operator()(const Point& point) const
    {
        return {
            std::sqrtf(point.x * point.x + point.y * point.y),
            std::atan2f(point.y, point.x)
        };
    }
};

int main()
{
    Point const p1Cartesian{0.1f, 10.0f};
    PolarPoint const p2Polar{10.0f, std::numbers::pi_v<float>/4.0f};

    PointExchanger const exchanger;

    PolarPoint const p1Polar = exchanger(p1Cartesian);
    Point const p2Cartesian = exchanger(p2Polar);

    std::cout << "P1" << std::endl
        << "  magnitude = " << p1Polar.magnitude << std::endl
        << "  angle     = " << p1Polar.angle << std::endl;

    std::cout << "P2" << std::endl
        << "  x = " << p2Cartesian.x << std::endl
        << "  y = " << p2Cartesian.y << std::endl;
}