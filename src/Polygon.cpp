#include "Polygon.h"
#include "Line.h"
#include "drawing.h"
#include <algorithm>

void scanFillPolygon(std::vector<Point> vertices, Color color)
{
    if (vertices.size() < 3)
        return;

    int minY = vertices[0].getY();
    int maxY = vertices[0].getY();
    for (Point v : vertices)
    {
        minY = std::min(minY, v.getY());
        maxY = std::max(maxY, v.getY());
    }

    for (int y = minY; y <= maxY; y++)
    {
        std::vector<int> intersections;

        for (size_t i = 0; i + 1 < vertices.size(); i++)
        {
            Point a = vertices[i];
            Point b = vertices[i + 1];
            int y1 = a.getY(), y2 = b.getY();

            if (y1 == y2)
                continue;

            int yMin = std::min(y1, y2);
            int yMax = std::max(y1, y2);

            if (y >= yMin && y < yMax)
            {
                double x = a.getX() + (double)(y - y1) * (b.getX() - a.getX()) / (y2 - y1);
                intersections.push_back((int)x);
            }
        }

        std::sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i + 1 < intersections.size(); i += 2)
        {
            for (int x = intersections[i]; x <= intersections[i + 1]; x++)
                setPixel(x, y, color);
        }
    }
}

Polygon::Polygon() : color(Color(0, 0, 0)) {}

Polygon::Polygon(std::vector<Point> vertices, Color color) : vertices(vertices), color(color) {}

Polygon::~Polygon() {}

void Polygon::setColor(Color color)
{
    this->color = color;
}

void Polygon::draw()
{
    if (this->vertices.size() < 2)
        return;

    if (this->filled)
        scanFillPolygon(this->vertices, this->fillColor);

    Point primeiro = this->vertices.front();
    Point anterior = this->vertices.front();
    Point atual;

    for (int i = 0; i < this->vertices.size() - 1; i++)
    {
        Point current = this->vertices[i];
        Point next = this->vertices[i + 1];
        Line(current, next, this->color).draw();
    }
}

void Polygon::addVertex(Point vertex)
{
    this->vertices.push_back(vertex);
}

Point Polygon::getFirstVertex()
{
    if (!this->vertices.empty())
    {
        return this->vertices.front();
    }
}

bool Polygon::contains(Point p)
{
    bool inside = false;
    size_t n = this->vertices.size();

    for (size_t i = 0, j = n - 1; i < n; j = i++)
    {
        int xi = vertices[i].getX(), yi = vertices[i].getY();
        int xj = vertices[j].getX(), yj = vertices[j].getY();

        bool intersects = ((yi > p.getY()) != (yj > p.getY())) &&
            (p.getX() < (double)(xj - xi) * (p.getY() - yi) / (double)(yj - yi) + xi);

        if (intersects)
            inside = !inside;
    }

    return inside;
}

void Polygon::setFill(Color fillColor)
{
    this->filled = true;
    this->fillColor = fillColor;
}
