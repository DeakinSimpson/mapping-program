#include "rtree.h"

#include <vector>
#include <cfloat>
#include <time.h>
#include <math.h>
/*
merges two halves back into one
*/
static void merge(long long *indicies, long long left, long long mid, long long right, Graph *g, int sort_by_lon, long long *temp)
{
    for (long long i = left; i < right; i++) temp[i] = indicies[i];

    long long i = left, j = mid, k = left;
    while (i < mid && j < right) {
        // give lat or lon depending on sort_by_lon variable
        double val_left  = sort_by_lon ? g->nodes[temp[i]].lon : g->nodes[temp[i]].lat;
        double val_right = sort_by_lon ? g->nodes[temp[j]].lon : g->nodes[temp[j]].lat;

        if (val_left <= val_right) {
            indicies[k++] = temp[i++];
        } else {
            indicies[k++] = temp[j++];
        }
    }
    while (i < mid)   indicies[k++] = temp[i++];
    while (j < right) indicies[k++] = temp[j++];
}

/*
will give indicies array which will be an index to the order of g

you can loop through g[indicies[i]] in order to get the sorted version

this 
*/
static void merge_sort(long long *indicies, long long left, long long right, Graph *g, int sort_by_lon, long long *temp)
{
    if (right - left <= 1) return;

    long long mid = (left + right) / 2;
    merge_sort(indicies, left, mid, g, sort_by_lon, temp);
    merge_sort(indicies, mid, right, g, sort_by_lon, temp);
    merge(indicies, left, mid, right, g, sort_by_lon, temp);
}

/*
gets the MBR for a node in a given graph
*/
static void compute_mbr(RTreeNode *node, Graph *g) {
    node->mbr.min_lat = DBL_MAX;
    node->mbr.max_lat = -DBL_MAX;
    node->mbr.min_lon = DBL_MAX;
    node->mbr.max_lon = -DBL_MAX;

    if (node->is_leaf)
    {
        for (int i = 0; i < node->children_count; i++)
        {
            double lat = g->nodes[node->entries[i]].lat;
            double lon = g->nodes[node->entries[i]].lon;

            if (lat < node->mbr.min_lat) node->mbr.min_lat = lat;
            if (lat > node->mbr.max_lat) node->mbr.max_lat = lat;
            if (lon < node->mbr.min_lon) node->mbr.min_lon = lon;
            if (lon > node->mbr.max_lon) node->mbr.max_lon = lon;            
        }
    } else
    {
        for (int i = 0; i < node->children_count; i++)
        {
            MinimumBoundingRectangle *c = &node->children[i]->mbr;

            if (c->min_lat < node->mbr.min_lat) node->mbr.min_lat = c->min_lat;
            if (c->max_lat > node->mbr.max_lat) node->mbr.max_lat = c->max_lat;
            if (c->min_lon < node->mbr.min_lon) node->mbr.min_lon = c->min_lon;
            if (c->max_lon > node->mbr.max_lon) node->mbr.max_lon = c->max_lon;
        }
    }
}

RTree* rtree_build(Graph *g) {
    long long node_count = g->nodes.size();

    /*
        --- step 1 ---
        sory all nodes by longitude
        [A, B, C, D, E, F, G, H, I]
    */

    // create the array indicies, which is the index to the graph g
    std::vector<long long> temp(node_count);
    std::vector<long long> indices(node_count);

    for (long long i = 0; i < node_count; i++) indices[i] = i;

    merge_sort(indices.data(), 0, node_count, g, 1, temp.data());

    /*
        --- step 2 ---
        split into column based on MAX_CHILDREN
        [A, B, C] [D, E, F] [G, H, I]
    */

    /*
    get the toal leaves (each leaf holds MAX_CHILDREN)

    to get a even distribution we get the square root as it splits it into a grid of leafes, this reduces overlap

    this results in the total number of slices needed to get a balances tree*/
    long long total_leaves = (node_count + MAX_CHILDREN - 1) / MAX_CHILDREN;
    long long total_nodes  = total_leaves;
    long long level_count  = total_leaves;

    while (level_count > 1) {
        level_count = (level_count + MAX_CHILDREN - 1) / MAX_CHILDREN;
        total_nodes += level_count;
    }

    RTree *tree = new RTree();
    tree->pool.resize(total_nodes);
    long long pool_used = 0;

    long long num_slice  = (long long)ceil(sqrt((double)total_leaves));
    long long slice_size = num_slice * MAX_CHILDREN;

    std::vector<RTreeNode*> leaves;
    leaves.reserve(total_leaves);

    for (long long slice = 0; slice < num_slice; slice++) {
        /*
            --- step 3 ---
            within each longitude slice, sory by latitude
            [A, B, C] sorted south to north
        */
        long long start = slice * slice_size;
        long long end   = start + slice_size;
        if (end > node_count) end = node_count;

        merge_sort(indices.data(), start, end, g, 0, temp.data());

        /*
            --- steo 4 ---
            group every MAX_CHILDREN nodes into leaves

            compute minimum bounding range
        */
        long long num_of_leaves = (end - start + MAX_CHILDREN - 1) / MAX_CHILDREN;
        for (long long leaf = 0; leaf < num_of_leaves; leaf++) {
            RTreeNode *cur_leaf = &tree->pool[pool_used++];
            cur_leaf->is_leaf = true;

            // get start and end position of the leaf, used to allocate choldren
            long long leaf_start = start + leaf * MAX_CHILDREN;
            long long leaf_end   = leaf_start + MAX_CHILDREN;

            // make sure leaf_end is not larger the slice
            if (leaf_end > end) leaf_end = end;

            // assign the children count to leaf
            cur_leaf->children_count = leaf_end - leaf_start;

            // assign the children to the leaf node
            for (long long child = 0; child < cur_leaf->children_count; child++) {
                cur_leaf->entries[child] = indices[leaf_start + child];
            }

            compute_mbr(cur_leaf, g);

            // add leaf to the leaf array
            leaves.push_back(cur_leaf);
        }
    }

    /*
        --- step 5 ---

        group leaves into internal nodes

        computer their mbr's

        repeat until at root
    */
    std::vector<RTreeNode*> cur_level = leaves;
    long long leaf_level_count = total_leaves;

    while (leaf_level_count > 1) {
        long long new_count = (leaf_level_count + MAX_CHILDREN - 1) / MAX_CHILDREN;
        std::vector<RTreeNode*> new_level(new_count);

        for (long long i = 0; i < new_count; i++) {
            RTreeNode *internal_node = &tree->pool[pool_used++];
            internal_node->is_leaf = false;
            internal_node->children_count = 0;

            long long current_pos = i * MAX_CHILDREN;

            // add children nodes to internal_node
            for (int j = 0; (j < MAX_CHILDREN) && (current_pos + j < leaf_level_count); j++) {
                internal_node->children[j] = cur_level[current_pos + j];
                internal_node->children_count++;
            }

            compute_mbr(internal_node, g);
            new_level[i] = internal_node;
        }

        cur_level = std::move(new_level);
        leaf_level_count = new_count;
    }

    tree->root = cur_level[0];

    return tree;
}

// gets the minimum distance from mbr to node squared
static double mbr_min_dist_sq(Coordinate coord, MinimumBoundingRectangle *mbr) {
    // set distances to 0
    double dlat = 0.0, dlon = 0.0;

    if      (coord.lat < mbr->min_lat) dlat = mbr->min_lat - coord.lat;
    else if (coord.lat > mbr->max_lat) dlat = coord.lat - mbr->max_lat;
    if      (coord.lon < mbr->min_lon) dlon = mbr->min_lon - coord.lon;
    else if (coord.lon > mbr->max_lon) dlon = coord.lon - mbr->max_lon;

    return dlat * dlat + dlon * dlon;    
}

static void rtree_nearest_node(RTreeNode *node, Coordinate coord, Graph *g, std::vector<AdjList> &adj, long long &best_index, double &best_dist) {
    if (node->is_leaf) {
        for (int i = 0; i < node->children_count; i++) {
            long long idx = node->entries[i];

            // check if the value has edges
            if (adj[idx].empty()) continue;

            // get distance in lat and lon
            double dlat = g->nodes[idx].lat - coord.lat;
            double dlon = g->nodes[idx].lon - coord.lon;

            // get distance
            double d = dlat * dlat + dlon * dlon;

            // check if it is closer
            if (d < best_dist) {
                best_dist = d;
                best_index = idx;
            }
        }
    } else {
        // check each node in the node if its internal
        for (int i = 0; i < node->children_count; i++) {
            // only do this if the node is within the square
            double d = mbr_min_dist_sq(coord, &node->children[i]->mbr);

            // if the distance is better then make it best
            if (d < best_dist) {
                rtree_nearest_node(node->children[i], coord, g, adj, best_index, best_dist);
            }
        }
    }
}

long long rtree_nearest(RTree *tree, Coordinate coord, Graph *g, std::vector<AdjList> &adj) {
    clock_t t = clock();

    long long best_index = -1;
    double best_dist = DBL_MAX;

    rtree_nearest_node(tree->root, coord, g, adj, best_index, best_dist);

    t = clock() - t;
    printf("time taken to find node rtree: %lld, %fs\n", best_index, (double)t / CLOCKS_PER_SEC);

    return best_index;
}