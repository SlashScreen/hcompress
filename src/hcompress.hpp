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
