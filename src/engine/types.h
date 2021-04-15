#ifndef TIDE_TYPES_H
#define TIDE_TYPES_H

#include <stdint.h>

typedef unsigned long long ulong;
typedef unsigned int       uint;
typedef unsigned short     ushort;
typedef unsigned char      uchar;

#include <glm/vec3.hpp>

struct COLOR
{
    glm::vec3 vec;
    uint hex;
    COLOR(uint hex)
    {
        this->hex = hex;
        vec.x = ((hex & 0xFF0000) >> 16) / 255.0;
        vec.y = ((hex & 0x00FF00) >>  8) / 255.0;
        vec.z = ((hex & 0x0000FF) >>  0) / 255.0;
    }
};

#endif // TIDE_TYPES_H