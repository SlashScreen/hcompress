#pragma once
#include "aabb.hpp"
#include "constants.hpp"

/*
This is a bespoke, heavily pared down implementation of a "Quadtree".
It is optimized for memory usage and cache hits, rather than functionality.
*/

namespace HCompressor
{
    typedef unsigned short treeindex; // A short should cover all planned resolutions.
    typedef char leafdepth;

    enum Quadrant
    {
        BL, // Bottom LEft
        TL, // Top Left
        TR, // Top Right
        BR, // Bottom Right
    };

    // A QuadTreeLeaf is a single square on a quadtree.
    struct QuadTreeLeaf
    {
    public:
        treeindex
            parent,      // Used for climbing up the tree from the leaves.
            first_child; // -1 if no children. Children are stored contiguously.
        leafdepth depth;

        QuadTreeLeaf();
        QuadTreeLeaf(leafdepth d, treeindex p);
    };

    // A Quadtree is a container for leaves.
    class QuadTree
    {
        QuadTreeLeaf leaves[RESERVE_SIZE]; // Pre-allocated array of leaves.
        treeindex index;                   // Functions a bit like a stack pointer; This is used to emulate a growing list.

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
