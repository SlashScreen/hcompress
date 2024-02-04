#pragma once
#include "constants.hpp"
#include "aabb.hpp"
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/object.hpp>

namespace HCompressor
{
    class Compressor : public godot::Object
    {
        GDCLASS(Compressor, Object)

        float **read_image(godot::Image *img); // Get heightmap array from image
        float **grab_span(float ***data, AABB *aabb);
        void regress(float ***data, float *a, float *xfac, float *yfac);

    public:
        void process_maps(godot::TypedArray<godot::Image> imgs);

    protected:
        static void _bind_methods();
    };
}
