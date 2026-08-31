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

// SDL stuff
SDL_Window *pWindow = nullptr;
SDL_Renderer *pRenderer = nullptr;
SDL_Surface *window_surface = nullptr;

int offset = 10;

// Global state
std::vector<Line> lines;
std::vector<Polygon> polygons;
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

bool drawPolygon = true;
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

void update()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        // selectedTool->handleEvent(event);

        drawLineHandler(event);
        drawRectangleHandler(event);
        drawPolygonHandler(event);

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

    renderLinePreview();
    renderRectanglePreview();
    renderPolygonPreview();
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

    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
        pWindow = SDL_CreateWindow("SDL Illustrator",
                                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   640, 480,
                                   SDL_WINDOW_SHOWN);

        if (pWindow != 0)
        {
            pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
            window_surface = SDL_GetWindowSurface(pWindow);
            Context *context = Context::getInstance();
            context->setRenderer(pRenderer);
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
