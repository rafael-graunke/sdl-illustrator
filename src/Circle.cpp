#include "Context.h"
#include "Circle.h"
#include "Line.h"
#include "drawing.h"

Circle::Circle() : center(Point(0, 0)), radius(0), color(Color(0, 0, 0)) {}

Circle::Circle(Point center, int radius, Color color) : center(center), radius(radius), color(color) {}

Circle::~Circle() {}

void Circle::setCenter(Point center)
{
    this->center = center;
}

void Circle::setRadius(int radius)
{
    this->radius = radius;
}

void Circle::setColor(Color color)
{
    this->color = color;
}


void displayBresenhamCircle(int xc,int yc, int x, int y, Color color) {
    setPixel(xc+x, yc+y, color);
    setPixel(xc-x, yc+y, color);
    setPixel(xc+x, yc-y, color);
    setPixel(xc-x, yc-y, color);
    setPixel(xc+y, yc+x, color);
    setPixel(xc-y, yc+x, color);
    setPixel(xc+y, yc-x, color);
    setPixel(xc-y, yc-x, color);
}

void Circle::draw()
{
    int x = 0, y = radius;
    int decesionParameter = 3 - 2 * radius;
    displayBresenhamCircle(center.getX(), center.getY(), x, y, this->color);
    while (y >= x)
    {
        x++;
        if (decesionParameter > 0)
        {
            y--;
            decesionParameter = decesionParameter + 4 * (x - y) + 10;
        }
        else
            decesionParameter = decesionParameter + 4 * x + 6;
        displayBresenhamCircle(center.getX(), center.getY(), x, y, this->color);
    }
}
