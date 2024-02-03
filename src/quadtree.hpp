#include "hcompress.hpp"

namespace HCompressor
{
    typedef treeindex unsigned short;
    typedef leafdepth unsigned char;

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

    enum Quadrant
    {
        BL,
        TL,
        TR,
        BR,
    };
}
