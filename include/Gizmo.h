#pragma once

#include <vector>
#include <Point.h>
#include <Color.h>
#include <Polygon.h>

class Gizmo
{
    public:
        Gizmo(int minX, int maxX, int minY, int maxY);
        Gizmo(std::vector<Point> vertices, Color color);
        virtual ~Gizmo();

        void translate(int dx, int dy);
        void rotate(double angle);
        void scale(double factor);

        void draw();
        
        Point getCenter();
        Polygon getContour();
        std::vector<Polygon> getCorners();
        std::vector<Polygon> getEdges();

    private:
        Polygon contour;
        std::vector<Polygon> corners;
        std::vector<Polygon> edges;
        Color color = Color(101, 113, 125);
        Color fillColor = Color(50, 147, 250);
};
