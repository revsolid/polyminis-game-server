#ifndef __TYPES
#define __TYPES

#include <math.h>

struct Coord
{
  float x;
  float y;
  static float Distance(Coord a, Coord b)
  {
      return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
  }
};
#endif
