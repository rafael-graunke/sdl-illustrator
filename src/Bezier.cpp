#include "Bezier.h"
#include "drawing.h"
#include <cmath>

Bezier::Bezier() {}
Bezier::Bezier(std::vector<Point> controlPoints, Color color) : controlPoints(controlPoints), color(color) {}

Bezier::~Bezier() {}

void Bezier::setControlPoints(std::vector<Point> controlPoints)
{
    this->controlPoints = controlPoints;
}

void Bezier::setColor(Color color)
{
    this->color = color;
}

void Bezier::setPoint(int index, Point p)
{
    if (index >= 0 && static_cast<size_t>(index) < controlPoints.size())
    {
        controlPoints[index] = p;
    }
}

std::vector<Point> Bezier::getControlPoints()
{
    return controlPoints;
}

int Bezier::nearestPoint(Point mouse, int threshold)
{
    int nearestIndex = -1;
    double nearestDistance = threshold;

    for (size_t i = 0; i < controlPoints.size(); ++i)
    {
        double distance = sqrt(pow(mouse.getX() - controlPoints[i].getX(), 2) + pow(mouse.getY() - controlPoints[i].getY(), 2));
        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestIndex = i;
        }
    }

    return nearestIndex;
}


void Bezier::draw()
{
    if (controlPoints.size() < 4)
        return; // Need at least 4 control points for a cubic Bezier curve

    Point p0 = controlPoints[0];
    Point p1 = controlPoints[1];
    Point p2 = controlPoints[2];
    Point p3 = controlPoints[3];

    for (double t = 0; t <= 1; t += 0.01)
    {
        double x = pow(1 - t, 3) * p0.getX() + 3 * pow(1 - t, 2) * t * p1.getX() + 3 * (1 - t) * pow(t, 2) * p2.getX() + pow(t, 3) * p3.getX();
        double y = pow(1 - t, 3) * p0.getY() + 3 * pow(1 - t, 2) * t * p1.getY() + 3 * (1 - t) * pow(t, 2) * p2.getY() + pow(t, 3) * p3.getY();
        setPixel(static_cast<int>(x), static_cast<int>(y), color);
    }
}