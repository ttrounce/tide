#ifndef TIDE_TYPES_H
#define TIDE_TYPES_H

#include <glm/vec3.hpp>
#include <stdint.h>
#include <memory>

typedef unsigned long long ulong;
typedef unsigned int       uint;
typedef unsigned short     ushort;
typedef unsigned char      uchar;


struct Bounds
{
    union
    {
        int arr[4];
        struct
        {
            int x1, y1, x2, y2;
        };
    };
    Bounds(int a, int b, int c, int d);
};

struct Rect
{
    union
    {
        int arr[4];
        struct
        {
            int x, y, w, h;
        };
    };
    Rect(int a, int b, int c, int d);
};

struct Color
{
    glm::vec3 vec;
    uint hex;
    Color(uint hex);
};

template <typename T>
using Shared = std::shared_ptr<T>;
template <typename T>
using Unique = std::unique_ptr<T>;
template <typename T>
using Weak = std::weak_ptr<T>;

#endif // TIDE_TYPES_H