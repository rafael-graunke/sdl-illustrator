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

void update()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        // selectedTool->handleEvent(event);

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

    SDL_UpdateWindowSurface(pWindow);
}

int main(int argc, char *args[])
{

    // Testing shape classes
    Line line = Line(Point(10, 10), Point(100, 100), Color(255, 0, 0));
    lines.push_back(line);

    Polygon polygon = Polygon({Point(200, 200), Point(300, 200), Point(300, 300), Point(200, 300), Point(150, 250) }, Color(0, 255, 0));
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
