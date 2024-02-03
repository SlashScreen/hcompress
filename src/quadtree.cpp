#include "quadtree.hpp"
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

AABB QuadTree::get_aabb_for(treeindex i)
{
    // If depth is 0, it's root, get full range
    QuadTreeLeaf *q = get(i);
    if (q->depth == 0)
    {
        return AABB(0, 0, MAP_DIMENSIONS, MAP_DIMENSIONS);
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
        return AABB(0, 0, MAP_DIMENSIONS, MAP_DIMENSIONS); // This should never happen.
    }
}

// Public

QuadTree::QuadTree()
{
    add_new_quad(0);
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
