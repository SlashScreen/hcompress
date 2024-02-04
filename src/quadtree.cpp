#include "quadtree.hpp"
#include <cstdlib>
using namespace HCompressor;

//* TREE

// Private

void QuadTree::add_new_quad(leafdepth d, treeindex p)
{
    QuadTreeLeaf leaf = QuadTreeLeaf(d, p);
    leaves[index] = leaf;
    index++;
}

QuadTreeLeaf *QuadTree::get(treeindex i)
{
    return &leaves[i];
}

Quadrant QuadTree::get_order(treeindex i)
{
    QuadTreeLeaf *c = get(i);
    QuadTreeLeaf *p = get(c->parent);
    // Measure distance between first child index and this leaf's index.
    // The child order goes BL -> TL -> TR -> BR.
    switch (i - p->first_child)
    {
    case 0:
        return Quadrant::BL;
    case 1:
        return Quadrant::TL;
    case 2:
        return Quadrant::TR;
    case 3:
        return Quadrant::BR;
    default:
        return Quadrant::BR; // This should never happen.
    }
}

// Public

QuadTree::QuadTree()
{
    leaves = new QuadTreeLeaf[0xFF];
}

QuadTree::QuadTree(int terrain_size)
{
    map_size = terrain_size;
    /*
        Find the number of times the quadtree must be subdivided to reach a minimum chunk size.
        The mathy way to do this, assuming map size is 1024 and chunk size is 16, is:
        1. 1024 / 16 = 64 (Max would be having 64x64 quads)
        2. log_base(2, 64) = 6 (Must divide 6 times to reach 64)

        Instead of doing that however, we will do a much more computery version: bit shifting!
        The old version used a lot of float casts and math.
        Don't worry, I tested this algorithm in Go first and this worked well.
    */
    int x = terrain_size;
    int depth_to_fit = 0;

    while (x != CHUNK_SIZE)
    {
        // Basically we are just counting how many shifts (equivalent to subdivisions) is between
        // the terrain size and the chunk size.
        x >>= 1;
        depth_to_fit++;
    }

    int new_size = (1 << ((depth_to_fit * 2) - 1)) - 1;
    // Simplified form of summing powers of 2, down from depth_to_fit. ex. 2^4 + 2^3 + ... + 2^0
    // This reasonably assumes all terrain sizes are powers of 2.

    leaves = new QuadTreeLeaf[new_size];
    add_new_quad(0);
}

QuadTree::~QuadTree()
{
    std::free(leaves);
}

void QuadTree::subdivide(treeindex i)
{
    QuadTreeLeaf *q = get(i);
    q->first_child = index; // Mark first child index for the subdividing quad.

    for (int i = 0; i < LEAVES; i++)
    {
        add_new_quad(q->depth + 1, i);
    }
}

QuadTreeLeaf *QuadTree::top()
{
    return &leaves[index - 1];
}

AABB QuadTree::get_aabb_for(treeindex i)
{
    // If depth is 0, it's root, get full range
    QuadTreeLeaf *q = get(i);
    if (q->depth == 0)
    {
        return AABB(0, 0, map_size, map_size);
    }

    // Calculate new AABB values
    AABB p_aabb = get_aabb_for(q->parent); // Is recursive
    int x = p_aabb.bottom_left_x;
    int y = p_aabb.bottom_left_y;
    int w = p_aabb.width >> 1; // Divides by 2.
    int h = p_aabb.height >> 1;

    // Construct AABBs based on their quadrant.
    switch (get_order(i))
    {
    case Quadrant::BL:
        return AABB(x, y, w, h);
    case Quadrant::TL:
        return AABB(x, y + h, w, h);
    case Quadrant::TR:
        return AABB(x + w, y + h, w, h);
    case Quadrant::BR:
        return AABB(x + w, y, w, h);
    default:
        return AABB(0, 0, map_size, map_size); // This should never happen.
    }
}

//* LEAF

// Public

QuadTreeLeaf::QuadTreeLeaf()
{
    parent = -1;
}

QuadTreeLeaf::QuadTreeLeaf(leafdepth d, treeindex p)
{
    depth = d;
    parent = p;
    first_child = -1;
}
