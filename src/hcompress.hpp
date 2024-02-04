#pragma once
#include "constants.hpp"
#include "aabb.hpp"
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/image.hpp>

namespace HCompressor
{
    class Compressor
    {
        float **read_image(godot::Image *img); // Get heightmap array from image
        float **grab_span(float ***data, AABB *aabb);

    public:
        void process_maps(godot::TypedArray<godot::Image> imgs);
    };
}
