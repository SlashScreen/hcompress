#pragma once
#include "constants.hpp"
#include "aabb.hpp"
#include "quadtree.hpp"
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/classes/ref.hpp>

/*
    This is written by SlashScreen. If there's any bugs in this code, message me about it, not Cory.

    This is a partial implemetation of Mohsen Zare's heightmap compression algorithm.
    He posted a video about it here: https://www.youtube.com/watch?v=5KkVftV-PTk

    The general gist is this:
    1. Break down a heightmap into a quadtree.
    2. For each quad, create a plane that passes through the middle of all of the points, giving a simplified profile
    of the area. By processing each height value relative to the plane, you get smaller height values.
    3. Using the smaller height values, you can do a lot of tricks to pack them in much smaller byte sizes.

    Zare's implementation has adjustable accuracy, but this is hard-coded to have centimeter accuracy.
    Theoretically, that's all anyone will need.
*/

namespace HCompressor
{
    enum EncodeType
    {
        Flat,
        U2,
        U4,
        U8,
        Full,
    };

    class Compressor : public godot::Object
    {
        GDCLASS(Compressor, Object)

        float **read_image(godot::Ref<godot::Image> img); // Get heightmap array from image
        float **grab_span(float **data, AABB *aabb);
        void regress(float **data, float *a, float *xfac, float *yfac, int dimension);
        float **adjust_span(float **data, float a, float xfac, float yfac, int dimension);
        godot::PackedByteArray process_quad(float **data, QuadTree *qt, int index, int dimension);
        godot::PackedByteArray float_to_bytes(float f);
        godot::PackedByteArray compress_image(godot::Ref<godot::Image> img);

    public:
        godot::TypedArray<godot::PackedByteArray> compress_images(godot::TypedArray<godot::Image> imgs);
        godot::PackedFloat32Array test_process_image(godot::Ref<godot::Image> img);

    protected:
        static void _bind_methods();
    };
}
