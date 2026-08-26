#pragma once

#include <Point.h>
#include <Color.h>

class Circle
{
    public:
        Circle();
        Circle(Point center, int radius, Color color);
        void setCenter(Point center);
        void setRadius(int radius);
        void setColor(Color color);
        void translate(int dx, int dy);
        void rotate(double angle);
        void scale(double factor);
        void draw();
        virtual ~Circle();

    private:
        Point center;
        int radius;
        Color color;

};
