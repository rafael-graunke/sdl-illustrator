#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

#include <Context.h>
#include <Color.h>
#include <Point.h>
#include <Line.h>
#include <Polygon.h>
#include <Circle.h>
#include <Bezier.h>
#include <cmath>

// SDL stuff
SDL_Window *pWindow = nullptr;
SDL_Surface *window_surface = nullptr;

int offset = 10;

// Global state
std::vector<Line> lines;
std::vector<Polygon> polygons;
std::vector<Bezier> beziers;
std::vector<Circle> circles;

void clear()
{
    Line l = Line();
    SDL_Surface *window_surface = Context::getInstance()->getWindowSurface();
    for (int x = 0; x < window_surface->w; x++)
    {
        for (int y = 0; y < window_surface->h; y++)
        {
            l.setPixel(x, y, 255, 255, 255);
        }
    }
}

bool drawLine = false;
Point *firstPoint = nullptr;
Point *secondPoint = nullptr;

void drawLineHandler(SDL_Event event)
{
    if (!drawLine)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        firstPoint = new Point(event.button.x, event.button.y);
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(event.motion.x, event.motion.y);
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (firstPoint != nullptr)
        {
            Line line = Line(*firstPoint, Point(event.button.x, event.button.y), Color(0, 0, 0));
            lines.push_back(line);
            delete firstPoint;
            firstPoint = nullptr;
            delete secondPoint;
            secondPoint = nullptr;
        }
    }
}

void renderLinePreview()
{
    if (!drawLine)
        return;

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        Line line = Line(*firstPoint, *secondPoint, Color(0, 0, 0));
        line.draw();
    }
}

bool drawRectangle = false;

void drawRectangleHandler(SDL_Event event)
{
    if (!drawRectangle)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        firstPoint = new Point(event.button.x, event.button.y);
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(event.motion.x, event.motion.y);
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (firstPoint != nullptr)
        {
            Polygon rectangle = Polygon(
                {*firstPoint, Point(firstPoint->getX(), event.button.y), Point(event.button.x, event.button.y), Point(event.button.x, firstPoint->getY())},
                Color(0, 0, 0));
            polygons.push_back(rectangle);
            delete firstPoint;
            firstPoint = nullptr;
            delete secondPoint;
            secondPoint = nullptr;
        }
    }
}

void renderRectanglePreview()
{
    if (!drawRectangle)
        return;

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        Polygon rectangle = Polygon(
            {*firstPoint, Point(firstPoint->getX(), secondPoint->getY()), *secondPoint, Point(secondPoint->getX(), firstPoint->getY())},
            Color(0, 0, 0)
        );
        rectangle.draw();
    }
}

// ====== Circle
bool drawCircle = true;

void drawCircleHandler(SDL_Event event)
{
    if (!drawCircle)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        firstPoint = new Point(event.button.x, event.button.y);
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(event.motion.x, event.motion.y);
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (firstPoint != nullptr)
        {
            // calculatging radius
            int radius = sqrt(pow(secondPoint->getX() - firstPoint->getX(), 2) + pow(secondPoint->getY() - firstPoint->getY(), 2));

            Circle circle = Circle(
                Point(firstPoint->getX(), firstPoint->getY()),
                radius,
                Color(0, 0, 0)
            );
            circles.push_back(circle);
            delete firstPoint;
            firstPoint = nullptr;
            delete secondPoint;
            secondPoint = nullptr;
        }
    }
}

void renderCirclePreview()
{
    if (!drawCircle)
        return;

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        int radius = sqrt(pow(secondPoint->getX() - firstPoint->getX(), 2) + pow(secondPoint->getY() - firstPoint->getY(), 2));
        Circle circle = Circle(*firstPoint, radius, Color(0, 0, 0));
        circle.draw();
    }
}
// ======

bool drawPolygon = false;
Polygon currentPolygon = Polygon();

void drawPolygonHandler(SDL_Event event)
{
    if (!drawPolygon)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if (firstPoint != nullptr)
            delete firstPoint;
        firstPoint = new Point(event.button.x, event.button.y);
        currentPolygon.addVertex(*firstPoint);
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(event.motion.x, event.motion.y);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        currentPolygon.addVertex(currentPolygon.getFirstVertex());
        polygons.push_back(currentPolygon);
        currentPolygon = Polygon();
        delete firstPoint;
        firstPoint = nullptr;
        delete secondPoint;
        secondPoint = nullptr;
    }
}

void renderPolygonPreview()
{
    if (!drawPolygon)
        return;

    currentPolygon.draw();

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        Line line = Line(*firstPoint, *secondPoint, Color(0, 0, 0));
        line.draw();
    }
}


bool drawBezier = false;
std::vector<Point> pendingPoints;
int draggingBezierIndex = -1;
int draggingPointIndex = -1;

void bezierPlacingHandler(SDL_Event event)
{
    if (!drawBezier)
        return;

    if (draggingPointIndex != -1)
        return; // clique foi pra iniciar um drag, nao pra marcar ponto novo

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        pendingPoints.push_back(Point(event.button.x, event.button.y));

        if (pendingPoints.size() == 4)
        {
            Bezier bezier = Bezier(pendingPoints, Color(0, 0, 0));
            beziers.push_back(bezier);
            pendingPoints.clear();
        }
    }
}

void bezierDragHandler(SDL_Event event)
{
    if (!drawBezier)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        Point mouse = Point(event.button.x, event.button.y);

        for (size_t i = 0; i < beziers.size(); ++i)
        {
            int index = beziers[i].nearestPoint(mouse, 10);
            if (index != -1)
            {
                draggingBezierIndex = i;
                draggingPointIndex = index;
                break;
            }
        }
    }

    if (event.type == SDL_MOUSEMOTION && draggingBezierIndex != -1)
    {
        beziers[draggingBezierIndex].setPoint(draggingPointIndex, Point(event.motion.x, event.motion.y));
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        draggingBezierIndex = -1;
        draggingPointIndex = -1;
    }
}

void renderBezierPreview()
{
    if (!drawBezier)
        return;

    for (size_t i = 0; i + 1 < pendingPoints.size(); ++i)
    {
        Line line = Line(pendingPoints[i], pendingPoints[i + 1], Color(0, 0, 0));
        line.draw();
    }
}

void renderBezierGuides()
{
    if (draggingBezierIndex == -1)
        return;

    std::vector<Point> points = beziers[draggingBezierIndex].getControlPoints();

    for (size_t i = 0; i + 1 < points.size(); ++i)
    {
        Line guide = Line(points[i], points[i + 1], Color(180, 180, 180));
        guide.draw();
    }
}

void update()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        // selectedTool->handleEvent(event);

        drawLineHandler(event);
        drawRectangleHandler(event);
        drawPolygonHandler(event);
        drawCircleHandler(event);
        bezierDragHandler(event);
        bezierPlacingHandler(event);

        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
    }
}

void render()
{
    clear();

    for (Line line : lines)
        line.draw();

    for (Polygon polygon : polygons)
        polygon.draw();

    for (Circle circle : circles)
        circle.draw();

    for (Bezier bezier : beziers)
        bezier.draw();

    renderLinePreview();
    renderRectanglePreview();
    renderPolygonPreview();
    renderCirclePreview();
    renderBezierPreview();
    renderBezierGuides();
    SDL_UpdateWindowSurface(pWindow);
}

int main(int argc, char *args[])
{

    // Testing shape classes
    Line line = Line(Point(10, 10), Point(100, 100), Color(255, 0, 0));
    lines.push_back(line);

    Polygon polygon = Polygon({Point(200, 200), Point(300, 200), Point(300, 300), Point(200, 300), Point(150, 250)}, Color(0, 255, 0));
    polygons.push_back(polygon);

    Circle circle = Circle(Point(400, 400), 50, Color(0, 0, 255));
    circles.push_back(circle);

    Bezier bezier = Bezier({Point(500, 500), Point(550, 450), Point(600, 550), Point(650, 500)}, Color(255, 255, 0));
    beziers.push_back(bezier);

    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
        pWindow = SDL_CreateWindow("SDL Illustrator",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   640, 480,
                                   SDL_WINDOW_SHOWN);

        if (pWindow != 0)
        {
            window_surface = SDL_GetWindowSurface(pWindow);
            Context *context = Context::getInstance();
            context->setWindowSurface(window_surface);
        }
    }
    else
        return EXIT_FAILURE;

    while (true)
    {
        update();
        render();

        // Throttle
        usleep(100);
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
