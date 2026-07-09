#include "graph.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

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

std::vector<AdjList> adjlist_create(Graph *g, std::unordered_map<long long, long long> &map, int reverse) {
    // allocate an adjacency list per node
    std::vector<AdjList> adj(g->nodes.size());

    for (long long i = 0; i < g->edge_count; i++) {
        Edge *e = &g->edges[i];

        // convert the OSM id into hashmap index
        long long src_index = reverse ? map.at(e->dst) : map.at(e->src);

        if (src_index == -1) {printf("failed to get hashmap for %lld", src_index); continue;}

        // modify the list in position src_index
        AdjList &list = adj[src_index];

        AdjEdge edge;
        edge.dst_index = reverse ? map.at(e->src) : map.at(e->dst);
        edge.weight    = e->weight;
        edge.km_weight = e->weight;
        edge.speed_limit = e->speed_limit;
        edge.road_type = e->road_type;

        list.push_back(edge);
    }

    return adj;
}

//TODO: Remove this as it is redundant
void adjlist_add_edge(std::vector<AdjList> &adj, long long src, long long dst, double weight, double km_weight, int speed_limit, RoadType road_type) {
    adj[src].push_back({dst, weight, km_weight, speed_limit, road_type});
}