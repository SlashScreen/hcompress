#pragma once

namespace HCompressor
{
    // This simple AABB is not used for collision, but rather to mark a region of an image.
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
