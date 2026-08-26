#include "Context.h"
#include "Color.h"
#include "drawing.h"


void setPixel(int x, int y, int r, int g, int b, int a) {
    unsigned int * pixels;
    SDL_Surface * window_surface = Context::getInstance()->getWindowSurface();
    pixels = (unsigned int *) window_surface->pixels;
    pixels[x + y * window_surface->w] = SDL_MapRGBA(window_surface->format, r, g, b, a);
}


void setPixel(int x, int y, int r, int g, int b) {
    setPixel(x, y, r, g, b, 255);
}

void setPixel(int x, int y, Color color) {
	setPixel(x, y, color.getR(),color.getG(),color.getB());
}

void setPixel(int x, int y, Uint32 cor) {
    Color c = Color();
    Uint8 r = c.getColorComponent(cor, 'r');
    Uint8 g = c.getColorComponent(cor, 'g');
    Uint8 b = c.getColorComponent(cor, 'b');
    setPixel(x, y, r, g, b);
}

