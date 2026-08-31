#include "Polygon.h"
#include "Line.h"

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
