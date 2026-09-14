#include "Gizmo.h"
#include "Polygon.h"
#include "Point.h"
#include <vector>


Gizmo::Gizmo(int minX, int maxX, int minY, int maxY) {
    std::vector<Point> corners;
    corners.push_back(Point(minX, minY));
    corners.push_back(Point(maxX, minY));
    corners.push_back(Point(maxX, maxY));
    corners.push_back(Point(minX, maxY));
    corners.push_back(Point(minX, minY));

    this->contour = Polygon(corners, this->color);

    for (size_t i = 0; i < corners.size() - 1; i++) {
        std::vector<Point> points;

        points.push_back(Point(corners[i].getX() - 3, corners[i].getY() - 3));
        points.push_back(Point(corners[i].getX() + 3, corners[i].getY() - 3));
        points.push_back(Point(corners[i].getX() + 3, corners[i].getY() + 3));
        points.push_back(Point(corners[i].getX() - 3, corners[i].getY() + 3));
        points.push_back(Point(corners[i].getX() - 3, corners[i].getY() - 3));

        this->corners.push_back(Polygon(points, Color(0, 0, 0, 0), this->fillColor));
    }

    std::vector<Point> edges;
    edges.push_back(Point((minX+maxX)/2, minY));
    edges.push_back(Point(maxX, (minY+maxY)/2));
    edges.push_back(Point((minX+maxX)/2, maxY));
    edges.push_back(Point(minX, (minY+maxY)/2));

    for (Point edge : edges) {
        std::vector<Point> points;

        points.push_back(Point(edge.getX() - 3, edge.getY() - 3));
        points.push_back(Point(edge.getX() + 3, edge.getY() - 3));
        points.push_back(Point(edge.getX() + 3, edge.getY() + 3));
        points.push_back(Point(edge.getX() - 3, edge.getY() + 3));
        points.push_back(Point(edge.getX() - 3, edge.getY() - 3));

        this->edges.push_back(Polygon(points, Color(0, 0, 0, 0), this->fillColor));
    }

}

Gizmo::~Gizmo() {}

Polygon Gizmo::getContour()
{
    return this->contour;
}
std::vector<Polygon> Gizmo::getCorners()
{
    return this->corners;
}
std::vector<Polygon> Gizmo::getEdges()
{
    return this->edges;
}

void Gizmo::translate(int dx, int dy)
{
    this->contour.translate(dx, dy);

    for (Polygon& pol : this->corners)
    {
        pol.translate(dx, dy);
    }

    for (Polygon& pol : this->edges)
    {
        pol.translate(dx, dy);
    }
}

void Gizmo::draw()
{
    this->contour.draw();
    for (Polygon pol : this->corners)
    {
        pol.draw();
    }

    for (Polygon pol : this->edges)
    {
        pol.draw();
    }
}
