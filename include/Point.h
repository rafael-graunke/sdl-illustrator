#pragma once

class Point
{
    public:
        Point();
        Point(int x, int y);
        int getX();
        int getY();
        void translate(int dx, int dy);
        void setX(int x);
        void setY(int y);
        void scale(double factor);
        void scale(double factorX, double factorY);
        double distance(Point point);
        virtual ~Point();

    protected:

    private:
        int x, y;
};
