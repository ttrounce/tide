#include "types.h"

Bounds::Bounds(int a, int b, int c, int d) : x1(a), y1(b), x2(c), y2(d)
{}

Rect::Rect(int a, int b, int c, int d) : x(a), y(b), w(c), h(d)
{}

Color::Color(uint hex)
{
    this->hex = hex;
    vec.x = ((hex & 0xFF0000) >> 16) / 255.0;
    vec.y = ((hex & 0x00FF00) >> 8) / 255.0;
    vec.z = ((hex & 0x0000FF) >> 0) / 255.0;
}