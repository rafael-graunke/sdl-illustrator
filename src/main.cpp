#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <cstdint>
#include <iostream>

#include <Context.h>
#include <Color.h>
#include <Point.h>
#include <Line.h>
#include <Polygon.h>
#include <Circle.h>
#include <Bezier.h>
#include <Gizmo.h>
#include <cmath>

// SDL stuff
SDL_Window *pWindow = nullptr;
SDL_Surface *window_surface = nullptr;
SDL_Surface *cleanCanvas = nullptr;

int offset = 10;

// Global state
std::vector<Line> lines;
std::vector<Polygon> polygons;
std::vector<Bezier> beziers;
std::vector<Circle> circles;

Line *selectedLine = nullptr;
Polygon *selectedPolygon = nullptr;
Circle *selectedCircle = nullptr;

Gizmo *gizmo = nullptr;

enum class ToolType
{
    Select,
    Line,
    Rectangle,
    Polygon,
    Circle,
    Bezier,
    Fill
};
ToolType currentTool = ToolType::Select;

struct ToolboxItem
{
    ToolType tool;
    int x,y,w=60,h=60;
};

std::vector<ToolboxItem> toolboxItems = {
    {ToolType::Select, 10, 10},
    {ToolType::Line, 10, 75},
    {ToolType::Rectangle, 10, 140},
    {ToolType::Polygon, 10, 205},
    {ToolType::Circle, 10, 270},
    {ToolType::Bezier, 10, 335},
    {ToolType::Fill, 10, 400},
};

// fonte bitmap 5x7: cada linha da letra e um byte, cada bit e um pixel (1 = aceso, 0 = apagado)
//https://voxelmanip.se/2025/01/16/drawing-text-in-the-sdl-renderer-without-sdl-ttf/
uint8_t letraS[7] = {0b01110, 0b10001, 0b10000, 0b01110, 0b00001, 0b10001, 0b01110};
uint8_t letraL[7] = {0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111};
uint8_t letraR[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10100, 0b10010, 0b10001};
uint8_t letraP[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10000, 0b10000, 0b10000};
uint8_t letraC[7] = {0b01111, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b01111};
uint8_t letraB[7] = {0b11110, 0b10001, 0b10001, 0b11110, 0b10001, 0b10001, 0b11110};
uint8_t letraF[7] = {0b11111, 0b10000, 0b10000, 0b11110, 0b10000, 0b10000, 0b10000};

void drawGlyph(Line &l, ToolType tool, int originX, int originY, int scale, int r, int g, int b)
{
    uint8_t *letra;

    switch (tool)
    {
        case ToolType::Select: letra = letraS; break;
        case ToolType::Line: letra = letraL; break;
        case ToolType::Rectangle: letra = letraR; break;
        case ToolType::Polygon: letra = letraP; break;
        case ToolType::Circle: letra = letraC; break;
        case ToolType::Bezier: letra = letraB; break;
        case ToolType::Fill: letra = letraF; break;
    }

    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            // testa o bit da coluna (5 bits, do mais significativo pro menos significativo)
            int bit = (letra[row] >> (4 - col)) & 1;
            if (bit == 0)
                continue;

            for (int sy = 0; sy < scale; sy++)
            {
                for (int sx = 0; sx < scale; sx++)
                {
                    l.setPixel(originX + col * scale + sx, originY + row * scale + sy, r, g, b);
                }
            }
        }
    }
}

void drawToolbox()
{
    Line l = Line();

    for (ToolboxItem item : toolboxItems)
    {
        int r, g, b;

        if (item.tool == currentTool)
        {
            r = 230; g = 200; b = 0;
        }
        else
        {
            r = 180; g = 180; b = 180;
        }

        for (int x = item.x; x < item.x + item.w; x++)
        {
            for (int y = item.y; y < item.y + item.h; y++)
            {
                l.setPixel(x, y, r, g, b);
            }
        }

        int scale = 4;
        int glyphW = 5 * scale;
        int glyphH = 7 * scale;
        int gx = item.x + (item.w - glyphW) / 2;
        int gy = item.y + (item.h - glyphH) / 2;
        drawGlyph(l, item.tool, gx, gy, scale, 30, 30, 30);
    }
}

bool toolboxClickHandler(SDL_Event event)
{
    if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
        return false;

    int mx = event.button.x;
    int my = event.button.y;

    for (ToolboxItem item : toolboxItems)
    {
        if (mx >= item.x && mx < item.x + item.w && my >= item.y && my < item.y + item.h)
        {
            currentTool = item.tool;
            return true;
        }
    }

    return false;
}

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

void saveToFile()
{
    if (!cleanCanvas)
        return;

    char fileName[256];
    int version = 0;

    // versioning
    while (true)
    {
        if (version == 0)
            snprintf(fileName, sizeof(fileName), "drawing.bmp");
        else
            snprintf(fileName, sizeof(fileName), "drawing%d.bmp", version);

        FILE *file = fopen(fileName, "rb");
        if (file == nullptr) {break;}

        fclose(file);
        version = version + 1;
    }

    if (SDL_SaveBMP(cleanCanvas, fileName) != 0)
        printf("erro ao salvar: %s\n", SDL_GetError());
    else
        printf("Imagem salva em %s\n", fileName);
}

// trava x,y dentro dos limites da tela (0..w-1, 0..h-1), pra nenhuma forma
// nascer com coordenada absurda quando o mouse eh arrastado pra fora da janela
Point clampPoint(int x, int y)
{
    SDL_Surface *window_surface = Context::getInstance()->getWindowSurface();

    if (x < 0) x = 0;
    if (x >= window_surface->w) x = window_surface->w - 1;

    if (y < 0) y = 0;
    if (y >= window_surface->h) y = window_surface->h - 1;

    return Point(x, y);
}

Point drakopoulosClip(Point origin, int rawX, int rawY)
{
    SDL_Surface *window_surface = Context::getInstance()->getWindowSurface();
    double xmin = 0, ymin = 0;
    double xmax = window_surface->w - 1;
    double ymax = window_surface->h - 1;

    double x1 = origin.getX(), y1 = origin.getY();
    double x2 = rawX, y2 = rawY; // ponto cru do mouse

    if (x2 < xmin)
    {
        // saiu pela esquerda
        y2 = y1 + (y2 - y1) / (x2 - x1) * (xmin - x1);
        x2 = xmin;
    }
    else if (x2 > xmax)
    {
        // saiu pela direita
        y2 = y1 + (y2 - y1) / (x2 - x1) * (xmax - x1);
        x2 = xmax;
    }

    if (y2 < ymin)
    {
        // saiu por cima
        x2 = x1 + (x2 - x1) / (y2 - y1) * (ymin - y1);
        y2 = ymin;
    }
    else if (y2 > ymax)
    {
        // saiu por baixo
        x2 = x1 + (x2 - x1) / (y2 - y1) * (ymax - y1);
        y2 = ymax;
    }

    return Point((int)x2, (int)y2);
}

Point *firstPoint = nullptr;
Point *secondPoint = nullptr;

void drawLineHandler(SDL_Event event)
{
    if (currentTool != ToolType::Line)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        firstPoint = new Point(clampPoint(event.button.x, event.button.y));
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(drakopoulosClip(*firstPoint, event.motion.x, event.motion.y));
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (firstPoint != nullptr)
        {
            Line line = Line(*firstPoint, drakopoulosClip(*firstPoint, event.button.x, event.button.y), Color(0, 0, 0));
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
    if (currentTool != ToolType::Line)
        return;

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        Line line = Line(*firstPoint, *secondPoint, Color(0, 0, 0));
        line.draw();
    }
}

void drawRectangleHandler(SDL_Event event)
{
    if (currentTool != ToolType::Rectangle)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        firstPoint = new Point(clampPoint(event.button.x, event.button.y));
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(clampPoint(event.motion.x, event.motion.y));
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (firstPoint != nullptr)
        {
            Point releasePoint = clampPoint(event.button.x, event.button.y);
            Polygon rectangle = Polygon(
                {*firstPoint, Point(firstPoint->getX(), releasePoint.getY()), releasePoint, Point(releasePoint.getX(), firstPoint->getY()), *firstPoint},
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
    if (currentTool != ToolType::Rectangle)
        return;

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        Polygon rectangle = Polygon(
            {*firstPoint, Point(firstPoint->getX(), secondPoint->getY()), *secondPoint, Point(secondPoint->getX(), firstPoint->getY()), *firstPoint},
            Color(0, 0, 0)
        );
        rectangle.draw();
    }
}

// ====== Circle
void drawCircleHandler(SDL_Event event)
{
    if (currentTool != ToolType::Circle)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        firstPoint = new Point(clampPoint(event.button.x, event.button.y));
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(clampPoint(event.motion.x, event.motion.y));
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
    if (currentTool != ToolType::Circle)
        return;

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        int radius = sqrt(pow(secondPoint->getX() - firstPoint->getX(), 2) + pow(secondPoint->getY() - firstPoint->getY(), 2));
        Circle circle = Circle(*firstPoint, radius, Color(0, 0, 0));
        circle.draw();
    }
}
// ======

Polygon currentPolygon = Polygon();

void drawPolygonHandler(SDL_Event event)
{
    if (currentTool != ToolType::Polygon)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if (firstPoint != nullptr)
            delete firstPoint;
        firstPoint = new Point(clampPoint(event.button.x, event.button.y));
        currentPolygon.addVertex(*firstPoint);
    }

    if (event.type == SDL_MOUSEMOTION && firstPoint != nullptr)
    {
        delete secondPoint;
        secondPoint = new Point(clampPoint(event.motion.x, event.motion.y));
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
    if (currentTool != ToolType::Polygon)
        return;

    currentPolygon.draw();

    if (firstPoint != nullptr && secondPoint != nullptr)
    {
        Line line = Line(*firstPoint, *secondPoint, Color(0, 0, 0));
        line.draw();
    }
}


std::vector<Point> pendingPoints;
int draggingBezierIndex = -1;
int draggingPointIndex = -1;

void bezierPlacingHandler(SDL_Event event)
{
    if (currentTool != ToolType::Bezier)
        return;

    if (draggingPointIndex != -1)
        return; // clique foi pra iniciar um drag, nao pra marcar ponto novo

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        pendingPoints.push_back(clampPoint(event.button.x, event.button.y));

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
    if (currentTool != ToolType::Select)
        return;

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        Point mouse = clampPoint(event.button.x, event.button.y);

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
        beziers[draggingBezierIndex].setPoint(draggingPointIndex, clampPoint(event.motion.x, event.motion.y));
    }

    if (event.type == SDL_MOUSEBUTTONUP)
    {
        draggingBezierIndex = -1;
        draggingPointIndex = -1;
    }
}

void renderBezierPreview()
{
    if (currentTool != ToolType::Bezier)
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

void fillHandler(SDL_Event event)
{
    if (currentTool != ToolType::Fill)
        return;

    if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
        return;

    Point clicked = clampPoint(event.button.x, event.button.y);
    Color fillColor = Color(220, 40, 40);

    for (int i = (int)circles.size() - 1; i >= 0; i--)
    {
        if (circles[i].contains(clicked))
        {
            circles[i].setFill(fillColor);
            return;
        }
    }

    for (int i = (int)polygons.size() - 1; i >= 0; i--)
    {
        if (polygons[i].contains(clicked))
        {
            polygons[i].setFill(fillColor);
            return;
        }
    }
}

Point *translateStart = nullptr;
bool isTranslating = false;

bool gizmoHandler(SDL_Event event) {
    // Retorna false quando o evento deve passar pros proximos handler

    Point clicked = clampPoint(event.button.x, event.button.y);

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        bool intersect = false;
        for (Polygon corner : gizmo->getCorners()) {
            if (corner.contains(clicked))
                intersect = true;
        }
        for (Polygon edge : gizmo->getEdges()) {
            if (edge.contains(clicked))
                intersect = true;
        }
        if (gizmo->getContour().contains(clicked))
        {
            intersect = true;
            isTranslating = true;
            if (translateStart == nullptr)
                translateStart = new Point(clicked.getX(), clicked.getY());
        }

        if (!intersect)
        {
            delete gizmo;
            delete translateStart;
            gizmo = nullptr;
            translateStart = nullptr;

            selectedLine = nullptr;
            selectedPolygon = nullptr;
            selectedCircle = nullptr;

            return false;
        }
    }

    if (event.type == SDL_MOUSEMOTION && isTranslating) {
        int dx = clicked.getX() - translateStart->getX();
        int dy = clicked.getY() - translateStart->getY();
        gizmo->translate(dx, dy);
        delete translateStart;
        translateStart = new Point(clicked.getX(), clicked.getY());

        if (selectedCircle != nullptr)
            selectedCircle->translate(dx, dy);

        if (selectedPolygon != nullptr)
            selectedPolygon->translate(dx, dy);
    }

    if (event.type == SDL_MOUSEBUTTONUP) {
        delete translateStart;
        translateStart = nullptr;
        isTranslating = false;
    }

    return true;
}


void selectHandler(SDL_Event event)
{
    if (currentTool != ToolType::Select)
        return;
    
    if (gizmo != nullptr) {
        if (gizmoHandler(event))
            return;
    }

    if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT)
        return;

    Point clicked = clampPoint(event.button.x, event.button.y);

    for (int i = (int)circles.size() - 1; i >= 0; i--)
    {
        if (circles[i].contains(clicked))
        {
            selectedCircle = &circles[i];
            gizmo = new Gizmo(
                selectedCircle->getMinX(),
                selectedCircle->getMaxX(),
                selectedCircle->getMinY(),
                selectedCircle->getMaxY()
            );
            return;
        }
    }

    for (int i = (int)polygons.size() - 1; i >= 0; i--)
    {
        if (polygons[i].contains(clicked))
        {
            selectedPolygon = &polygons[i];
            gizmo = new Gizmo(
                selectedPolygon->getMinX(),
                selectedPolygon->getMaxX(),
                selectedPolygon->getMinY(),
                selectedPolygon->getMaxY()
            );
            return;
        }
    }

    selectedLine = nullptr;
    selectedPolygon = nullptr;
    selectedCircle = nullptr;
    delete gizmo;
    gizmo = nullptr;
}


void update()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        // selectedTool->handleEvent(event);

        if (toolboxClickHandler(event))
            continue;

        drawLineHandler(event);
        drawRectangleHandler(event);
        drawPolygonHandler(event);
        drawCircleHandler(event);
        bezierDragHandler(event);
        bezierPlacingHandler(event);
        fillHandler(event);
        selectHandler(event);


        if(event.type == SDL_KEYDOWN)
        {

            if (event.key.keysym.sym == SDLK_s)
            {
                saveToFile();
            }

            switch (event.key.keysym.sym)
            {
            case SDLK_s:
                currentTool = ToolType::Select;
                break;
            case SDLK_l:
                currentTool = ToolType::Line;
                break;
            case SDLK_r:
                currentTool = ToolType::Rectangle;
                break;
            case SDLK_p:
                currentTool = ToolType::Polygon;
                break;
            case SDLK_c:
                currentTool = ToolType::Circle;
                break;
            case SDLK_f:
                currentTool = ToolType::Fill;
                break;
            case SDLK_b:
                currentTool = ToolType::Bezier;
                break;
            default:
                break;
            }
        }

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

    if (gizmo != nullptr)
        gizmo->draw();

    if (cleanCanvas)
        SDL_FreeSurface(cleanCanvas);
    cleanCanvas = SDL_ConvertSurface(window_surface, window_surface->format, 0);

    drawToolbox();
    SDL_UpdateWindowSurface(pWindow);

}

int main(int argc, char *args[])
{
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
