#include "hcompress.hpp"

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

float **Compressor::grab_span(float ***data, AABB *aabb)
{
    float **arr = new float *[aabb->width];

    for (size_t x = 0; x < aabb->width; x++)
    {
        arr[x] = new float[aabb->height];

        for (size_t y = 0; y < aabb->height; y++)
        {
            arr[x][y] = *data[x][y]; // simply copying the data is I Think less bytes than using pointers
        }
    }

    return arr;
}

void Compressor::regress(float ***data, float *a, float *xfac, float *yfac)
{
    float sum = 0.0;
}

// public

void Compressor::process_maps(TypedArray<Image> imgs)
{
}
