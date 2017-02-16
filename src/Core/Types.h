#ifndef __TYPES
#define __TYPES

#include <math.h>

struct Coord
{
    float x;
    float y;
    Coord() = default;
    Coord(float x, float y) : x(x), y(y) {}
    static float Distance(Coord a, Coord b)
    {
      return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    }
};

template<typename T>
struct Range
{
    T Min;
    T Max;

    Range() = default;

    Range<T>(T min, T max) : Min(min), Max(max)
    {
    }

    inline bool Within(T value)
    {
        return (Min <= value && value <= Max); 
    }
};
#endif
