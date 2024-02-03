#pragma once
#include "aabb.hpp"
#include "hcompress.hpp"

namespace HCompressor
{
    typedef unsigned short treeindex; // A short should cover all planned resolutions.
    typedef unsigned char leafdepth;

    enum Quadrant
    {
        BL,
        TL,
        TR,
        BR,
    };

    struct QuadTreeLeaf
    {
    public:
        treeindex
            parent,
            first_child;
        leafdepth depth;
        QuadTreeLeaf();
        QuadTreeLeaf(leafdepth d, treeindex p);
    };

    class QuadTree
    {
        QuadTreeLeaf leaves[RESERVE_SIZE];
        treeindex index;
        void add_new_quad(leafdepth d, treeindex p = 0);
        QuadTreeLeaf *get(treeindex i);
        Quadrant get_order(treeindex i);
        AABB get_aabb_for(treeindex i);

    public:
        QuadTree();
        void subdivide(treeindex i);
        QuadTreeLeaf *top();
    };
}
