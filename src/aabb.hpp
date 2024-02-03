#pragma once

namespace HCompressor
{
    struct AABB
    {
        int
            bottom_left_x,
            bottom_left_y,
            width,
            height;

    public:
        AABB();
        AABB(int x, int y, int w, int h);
        bool contains_point(int x, int y);
    };
}
