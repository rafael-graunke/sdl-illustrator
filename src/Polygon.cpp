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
    Point primeiro = this->vertices.front();
    Point anterior = this->vertices.front();
    Point atual;

    int i = 0;
    for (Point p : this->vertices)
    {
        if (i > 0)
        {
            atual = p;
            Line(anterior, atual, this->color).draw();
            anterior = atual;
        }
        i++;
    }
    Line(atual, primeiro, this->color).draw();
}
