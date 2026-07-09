#pragma once

#include "graph.h"
#include "rtree.h"
#include "ch.h"
#include <unordered_map>
#include <string>

struct Place {
    const char *name;
    Coordinate coord;
};

struct LoadedVariables {
    Graph   *g;
    std::unordered_map<long long, long long> map;
    std::vector<AdjList> adj;
    std::vector<AdjList> adj_r;
    CHGraph *ch_g;
    RTree   *tree;
    bool     loaded_from_cache;
};

extern Place PLACES[];
extern int PLACES_COUNT;

void utils_parse_arg(Coordinate *src_coord, Coordinate *dst_coord, char *argv[]);
void utils_parse_places(Coordinate *src_coord, Coordinate *dst_coord, char *argv[]);
void utils_get_coord(Coordinate *src_coord, Coordinate *dst_coord, char *argv[]);
void utils_get_index(long long *src_index, long long *dst_index, char *argv[], RTree *tree, Graph *g, std::vector<AdjList> adj);
void utils_print_results(ResultPath *rp);
LoadedVariables utils_load_variables(const char *bin_path);

