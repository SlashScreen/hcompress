#pragma once

namespace HCompressor
{
    const int MAP_DIMENSIONS = 1024;
    const int LEAVES = 4;
    const int MAX_DEPTH = 4;
    const int RESERVE_SIZE = (1 << ((MAX_DEPTH * 2) - 1)) - 1;
    /*
    This is *In Theory* the maximum number of quads created if all quads were fully subdivided to maximum depth.
    It's a simplified form of 2^4 + 2^3 + ... + 2^0.
    */
}
