#pragma once

namespace HCompressor
{
    const int CHUNK_SIZE = 16;
    const int LEAVES = 4;
    // Subdivision
    const float SUBDIVIDE_THRESHOLD = 8.0;
    // Encoding
    const unsigned char QUAD_HEADER = 0b10101010;
    const float FLAT_TRESHOLD = 0.01;
    const float U2_THRESHOLD = 0.04;
    const float U4_THRESHOLD = 0.16;
    const float U8_THRESHOLD = 2.55;
}
