#include "graph.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>

Graph *graph_load(const char* path) {
    // open the binary
    FILE *f = fopen(path, "rb");
    if (!f) {
        std::cout << "Failed to open bin" << std::endl;
        return nullptr;
    }

    // read node count
    long long node_count, edge_count;   
    fread(&node_count, sizeof(long long), 1, f);
    fread(&edge_count, sizeof(long long), 1, f);

    // initialise Graph with counts
    Graph *g = new Graph();
    g->nodes.resize(node_count);
    g->edges.resize(edge_count);

    // read all nodes & edges
    // as each chunk is exactly the size of a node, we can just set the nodes to the binary
    fread(g->nodes.data(), sizeof(Node), node_count, f);
    fread(g->edges.data(), sizeof(Edge), edge_count, f);

    // sets the node and edge count of the graph
    g->node_count = node_count;
    g->edge_count = edge_count;

    // close file
    fclose(f);

    // return graph
    return g;
}

// void result_path_free(ResultPath *rp) {
//     free(rp->path_inx);
//     free(rp);
// }