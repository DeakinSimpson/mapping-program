#pragma once

#include "graph.h"
#include <array>
/*
Minimum bounding box for the R-Tree

If the node is outside of this area, it will not be included in the tree
*/
struct MinimumBoundingRectangle {
    double min_lat;
    double max_lat;
    double min_lon;
    double max_lon;
};

/*
each node in the tree has its own Minimum Bounding Area for all the nodes that it contains

each node is either a leaf, or an internal node (not a leaf)

children count is the number of children that this node has

MAX_CHILDREN is the maximum number of children nodes that the node can have
    if this is too small the tree will be deep

    if it is too big the tree will be too shallow

all internal nodes contain other R-tree nodes

whereas leaf nodes contain map/road nodes
*/
#define MAX_CHILDREN 16

struct RTreeNode {
    MinimumBoundingRectangle mbr;
    bool is_leaf;
    int children_count;
    std::array<RTreeNode*, MAX_CHILDREN> children;
    std::array<long long, MAX_CHILDREN> entries;
};

/*
root is just a pointer to the root of the tree

size is the total number of nodes in the map tree 

(this is because some can get disregarded if they are outside the MBR)
*/
struct RTree {
    RTreeNode *root;
    std::vector<RTreeNode> pool;
};

RTree* rtree_build(Graph *g);
long long rtree_nearest(RTree *tree, Coordinate coord, Graph *g, std::vector<AdjList> &adj);
