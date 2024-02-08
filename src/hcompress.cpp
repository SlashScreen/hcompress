#include "hcompress.hpp"
#include <cmath>

using namespace HCompressor;
using namespace godot;

float **Compressor::read_image(Ref<Image> img)
{
    int w = img->get_width();
    int h = img->get_height();
    float **arr = new float *[w];

    for (size_t x = 0; x < w; x++)
    {
        arr[x] = new float[h];

        for (size_t y = 0; y < h; y++)
        {
            arr[x][y] = img->get_pixel(x, y).r; // Read red channel
        }
    }

    return arr;
}

float **Compressor::grab_span(float **data, AABB *aabb)
{
    float **arr = new float *[aabb->width];

    for (size_t x = 0; x < aabb->width; x++)
    {
        arr[x] = new float[aabb->height];

        for (size_t y = 0; y < aabb->height; y++)
        {
            arr[x][y] = data[x][y]; // simply copying the data is I Think less bytes than using pointers
        }
    }

    return arr;
}

// Planar regress data.
void Compressor::regress(float **data, float *a, float *xfac, float *yfac, int dimension)
{
    // Implementation of: https://www.freecodecamp.org/news/the-least-squares-regression-method-explained/

    // Step 1: gather sums
    const int _x = 0;
    const int _y = 1;
    const int _z = 2;

    float sum[3];
    // Step 1
    {
        sum[_x] = (float)(dimension / 2);
        sum[_y] = (float)(dimension / 2);

        for (size_t x = 0; x < dimension; x++)
        {
            for (size_t y = 0; y < dimension; y++)
            {
                sum[_z] += data[x][y];
            }
        }

        sum[_z] /= (float)(dimension * dimension);
    }
    // Step 2
    const int _xx = 0;
    const int _yy = 1;
    const int _xz = 2;
    const int _yz = 3;
    float sigma[4];
    {
        for (size_t x = 0; x < dimension; x++)
        {
            sigma[_xx] += std::pow(x - sum[_x], 2);
            for (size_t y = 0; y < dimension; y++)
            {
                float z = data[x][y];
                sigma[_yy] += std::pow(y - sum[_y], 2);
                sigma[_xz] += (x - sum[_x]) * (z - sum[_z]);
                sigma[_yz] += (y - sum[_y]) * (z - sum[_z]);
            }
        }

        *xfac = sigma[_xz] / sigma[_xx];
        *yfac = sigma[_yz] / sigma[_yy];

        *a = sum[_z] - ((*xfac * sum[_x]) + (*yfac * sum[_y]));
    }
}

// Get relative values of a span to plane.
float **Compressor::adjust_span(float **data, float a, float xfac, float yfac, int dimension)
{
    float **arr = new float *[dimension];
    for (size_t x = 0; x < dimension; x++)
    {
        for (size_t y = 0; y < dimension; y++)
        {
            arr[x][y] = data[x][y] - (a + (xfac * x) + (yfac * y));
        }
    }
    return arr;
}

PackedByteArray Compressor::process_quad(float **data, QuadTree *qt, int index, int dimension)
{
    PackedByteArray output = PackedByteArray();

    AABB span_box = qt->get_aabb_for(index);
    float **span = grab_span(data, &span_box);
    float a;
    float xfac;
    float yfac;
    regress(span, &a, &xfac, &yfac, dimension);
    float **adjusted = adjust_span(span, a, xfac, yfac, dimension);
    // Determine value range
    float min = 0.0;
    float max = 0.0;
    for (size_t x = 0; x < dimension; x++)
    {
        for (size_t y = 0; y < dimension; y++)
        {
            min = std::min(min, adjusted[x][y]);
            max = std::min(max, adjusted[x][y]);
        }
    }
    float diff = max - min;
    output.append(QUAD_HEADER);
    output.append(qt->get(index)->depth); // Depth will determine dimensions for reading, so we don't need to store it
    // Store plane
    output.append_array(float_to_bytes(a));
    output.append_array(float_to_bytes(xfac));
    output.append_array(float_to_bytes(yfac));

    if (diff < FLAT_TRESHOLD)
    {
        output.append(EncodeType::Flat);
        // We don't need to include any more data.
    }
    else if (diff < U2_THRESHOLD)
    {
        output.append(EncodeType::U2);
        /*
            Here we loop through the array, take out chunks of 4 height values, and then squish them into a single byte.
            We have centimeter precision here, so this is only if the maximum variance in the terrain is 0.3.
            Assuming this, we store each height as an index into the range 0.00 - 0.04, with a step of 0.1.
        */
        for (size_t x = 0; x < dimension; x++) // Each row
        {
            for (size_t i = 0; i < dimension; i += 4) // each byte
            {
                unsigned char new_byte = 0;
                for (size_t b = 0; b < 4; b++) // each float
                {
                    float val = data[x][i + b] - min; // relative offset from minimum
                    int place = (int)(val * 100);     // whether it's index 0, 1, 2, 3
                    new_byte |= place << (b * 2);     // place in 1 - 3, offset by position in byte, * 2 for U2
                    // It will be placed into the byte in the order 3210, but that won't matter if it's read in the same order
                }
                output.append(new_byte);
            }
        }
    }
    else if (diff < U4_THRESHOLD)
    {
        output.append(EncodeType::U4);
        // This is the same process as U2, but packing 2 floats into a byte, with a range of 0.0 - 0.15.
        for (size_t x = 0; x < dimension; x++) // Each row
        {
            for (size_t i = 0; i < dimension; i += 2) // each byte
            {
                unsigned char new_byte = 0;
                for (size_t b = 0; b < 2; b++) // each float
                {
                    float val = data[x][i + b] - min; // relative offset from minimum
                    int place = (int)(val * 100);     // index
                    new_byte |= place << (b * 4);     // place in 1 - 3, offset by position in byte, * 2 for U2
                    // It will be placed into the byte in the order 3210, but that won't matter if it's read in the same order
                }
                output.append(new_byte);
            }
        }
    }
    else if (diff < U8_THRESHOLD)
    {
        output.append(EncodeType::U8);
        // This is a similar idea as U4, but no byte chunks.
        for (size_t x = 0; x < dimension; x++) // Each row
        {
            for (size_t y = 0; y < dimension; y++) // each byte
            {
                float val = data[x][y] - min;                        // relative offset from minimum
                unsigned char new_byte = (unsigned char)(val * 100); // index
                output.append(new_byte);
            }
        }
    }
    else
    {
        output.append(EncodeType::Full);
        // Worst case scenario. Store all f32s.
        for (size_t x = 0; x < dimension; x++) // Each row
        {
            for (size_t y = 0; y < dimension; y++) // each byte
            {
                output.append_array(float_to_bytes(data[x][y]));
            }
        }
    }

    std::free(span);
    std::free(adjusted);
    return output;
}

PackedByteArray Compressor::float_to_bytes(float f)
{
    unsigned char *p = reinterpret_cast<unsigned char *>(&f);
    PackedByteArray arr = PackedByteArray();

    for (size_t i = 0; i < sizeof(float); i++)
    {
        arr.append(p[i]);
    }
    std::free(p);

    return arr;
}

PackedByteArray Compressor::compress_image(Ref<Image> img)
{
    float **height_data = read_image(img);
    float min, max;
    QuadTree qt = QuadTree(img->get_width());
    /*
        The MTerrain algorithm actually fully subdivides first and then un-subdivides to find the best compression for the area.
        My implementation does not do this, because my half-featured quadtree implementation doesn't support it.
        I *could* create a sort of switchback array or whatever it's called, but I don't feel like doing that.
    */
    for (size_t x = 0; x < img->get_width(); x++)
    {
        for (size_t y = 0; y < img->get_height(); y++)
        {
            // Subdivide if min and max is too high
            float h = height_data[x][y];
            min = std::min(min, h);
            max = std::max(max, h);
            if (max - min > SUBDIVIDE_THRESHOLD)
            {
                qt.subdivide(qt.lowest_quad_under(x, y));
                min, max = 0;
            }
        }
    }

    PackedByteArray arr = PackedByteArray();
    for (size_t i = 0; i < qt.size(); i++)
    {
        arr.append_array(process_quad(height_data, &qt, i, img->get_width()));
    }

    std::free(height_data);
    return arr.compress();
}

// public

TypedArray<PackedByteArray> Compressor::compress_images(TypedArray<Image> imgs)
{
    TypedArray<PackedByteArray> arr = TypedArray<PackedByteArray>();
    for (size_t i = 0; i < imgs.size(); i++)
    {
        Ref<Image> img = imgs[i];
        arr.append(compress_image(img));
    }

    return arr;
}

PackedFloat32Array Compressor::test_process_image(Ref<Image> img)
{
    float **d = read_image(img);
    PackedFloat32Array arr = PackedFloat32Array();
    for (size_t x = 0; x < img->get_width(); x++)
    {
        for (size_t y = 0; y < img->get_height(); y++)
        {
            arr.push_back(d[x][y]);
        }
    }
    std::free(d);
    return arr;
}

// protected

void Compressor::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("test_process_image", "image"), &Compressor::test_process_image);
    ClassDB::bind_method(D_METHOD("compress_images", "heightmaps"), &Compressor::compress_images);
}
