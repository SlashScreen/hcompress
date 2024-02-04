#include "hcompress.hpp"
#include <cmath>

using namespace HCompressor;
using namespace godot;

float **Compressor::read_image(Image *img)
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

    float **span = grab_span(data, &(qt->get_aabb_for(index)));
    float a;
    float xfac;
    float yfac;
    regress(span, &a, &xfac, &yfac, dimension);
    float **adjusted = adjust_span(span, a, xfac, yfac, dimension);

    /*
    TODO:
    - Determine compression method
    - Make header, depth, etc
    - Compress floats (or do flat)
    - Append to byte array
    */

    std::free(span);
    std::free(adjusted);
    return output;
}

// public

godot::PackedByteArray Compressor::compress_images(TypedArray<Image> *imgs)
{
    return PackedByteArray();
}

PackedFloat32Array Compressor::test_process_image(Image *img)
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
}
