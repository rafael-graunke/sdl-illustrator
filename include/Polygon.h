#pragma once

#include <vector>
#include <Point.h>
#include <Color.h>

class Polygon
{
    public:
        Polygon();
        Polygon(std::vector<Point> vertices, Color color);
        void setColor(Color color);
        void translate(int dx, int dy);
        void rotate(double angle);
        void scale(double factor);
        void addVertex(Point vertex);
        void draw();
        virtual ~Polygon();

    private:
        std::vector<Point> vertices;
        Color color;

};
