#include "Context.h"
#include "Circle.h"
#include "Line.h"
#include "drawing.h"
#include <cmath>

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


void scanFillCircle(Point center, int radius, Color color)
{
    int cx = center.getX();
    int cy = center.getY();

    for (int y = -radius; y <= radius; y++)
    {
        int dx = (int)sqrt((double)(radius * radius - y * y));
        for (int x = -dx; x <= dx; x++)
            setPixel(cx + x, cy + y, color);
    }
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
    if (this->filled)
        scanFillCircle(this->center, this->radius, this->fillColor);

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

bool Circle::contains(Point p)
{
    int dx = p.getX() - center.getX();
    int dy = p.getY() - center.getY();
    return dx * dx + dy * dy <= radius * radius;
}

void Circle::setFill(Color fillColor)
{
    this->filled = true;
    this->fillColor = fillColor;
}
