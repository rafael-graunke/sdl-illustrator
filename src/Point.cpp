#include "Point.h"
#include <cmath>

Point::Point()
{
    //ctor
}

Point::Point(int x, int y)
{
    this->x = x;
    this->y = y;
}

Point::~Point()
{
    //dtor
}

void Point::translate(int dx, int dy)
{
    this->x += dx;
    this->y += dy;
}

int Point::getX()
{
    return this->x;
}

int Point::getY()
{
    return this->y;
}

void Point::setX(int x)
{
    this->x = x;
}

void Point::setY(int y)
{
    this->y = y;
}

void Point::scale(double factor)
{
    this->x *= factor;
    this->y *= factor;
}

void Point::scale(double factorX, double factorY)
{
    this->x *= factorX;
    this->y *= factorY;
}

double Point::distance(Point point) {
    double a = std::pow(this->getX() - point.getX(), 2);
    double b = std::pow(this->getY() - point.getY(), 2);
    double c = std::sqrt(a + b);
    return std::abs(c);
}
