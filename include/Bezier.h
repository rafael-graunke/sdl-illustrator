#pragma once

#include <vector>
#include <Point.h>
#include <Color.h>


class Bezier
{
    public:
        Bezier();
        Bezier(std::vector<Point> controlPoints, Color color);
        void setControlPoints(std::vector<Point> controlPoints);
        void setColor(Color color);
        void draw();
        virtual ~Bezier();

        void setPoint(int index, Point p);
        int nearestPoint(Point mouse, int threshold);
        std::vector<Point> getControlPoints();

    private:
        std::vector<Point> controlPoints;
        Color color;
};

