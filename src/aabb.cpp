#include "aabb.hpp"
using namespace HCompressor;

//* AABB

// Public

AABB::AABB(int x, int y, int w, int h)
{
    bottom_left_x = x;
    bottom_left_y = y;
    width = w;
    height = h;
}

bool AABB::contains_point(int x, int y)
{
    return (AABB::bottom_left_x <= x && x <= AABB::bottom_left_x + AABB::width) &&
           (AABB::bottom_left_y <= y && y <= AABB::bottom_left_y + AABB::height);
}
