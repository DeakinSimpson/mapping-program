#pragma once

#include "graph.h"
#include <unordered_map>
#include <queue>
#include <vector>

struct CHGraph {
    Graph *g;
    std::vector<long long> rank;
};

using HeapEntry = std::pair<double, long long>;
using MinHeap   = std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>>;

CHGraph *ch_build(Graph *g, std::vector<AdjList> &adj, std::vector<AdjList> &adj_r);
ResultPath ch_query(Graph *g, CHGraph *ch_g, std::vector<AdjList> &adj, std::vector<AdjList> &adj_r, std::unordered_map<long long, long long> &map, long long src_id, long long dst_id);
int ch_save(const char *path, Graph *g, std::vector<AdjList> &adj, std::vector<AdjList> &adj_r, CHGraph *ch_g);
CHGraph *ch_load(const char *path, Graph *g, std::vector<AdjList> &out_adj, std::vector<AdjList> &out_adj_r);