#include "utils.h"
#include "ch.h"
#include <cstring>
#include <time.h>
#include <iostream>

// Define places
Place PLACES[] = {
    {"Venice", {45.441310241560494, 12.31523127982292}},
    {"Vicenza", {45.54745520724044, 11.547651470387395}},
    {"Rome", {41.89107596300499, 12.492670206083258}},
    {"malta-east", {35.86126395007788, 14.571882644990628}},
    {"malta-west", {35.958249006888245, 14.365918959624256}},
    {"Traralgon", {-38.195017105603625, 146.53809419732184}},
    {"Melbourne", {-37.81746326177289, 144.9674458085295}}
};

int PLACES_COUNT = 7;

void utils_parse_arg(Coordinate *src_coord, Coordinate *dst_coord, char *argv[]) {
    src_coord->lat = atof(argv[3]);
    src_coord->lon = atof(argv[4]);

    dst_coord->lat = atof(argv[5]);
    dst_coord->lon = atof(argv[6]);
}

void utils_parse_places(Coordinate *src_coord, Coordinate *dst_coord, char *argv[]) {
    // check for src_coord
    for (int i = 0; i < PLACES_COUNT; i++)
    {
        if (strcmp(PLACES[i].name, argv[3]) == 0)
        {
            src_coord->lat = PLACES[i].coord.lat;
            src_coord->lon = PLACES[i].coord.lon;
        }
    }

    // check for dst_coord
    for (int i = 0; i < PLACES_COUNT; i++)
    {
        if (strcmp(PLACES[i].name, argv[4]) == 0)
        {
            dst_coord->lat = PLACES[i].coord.lat;
            dst_coord->lon = PLACES[i].coord.lon;
        }
    }    
}

void utils_get_coord(Coordinate *src_coord, Coordinate *dst_coord, char *argv[]) {
    if (strcmp(argv[2], "-c") == 0)
    {
        utils_parse_arg(src_coord, dst_coord, argv);
    } else
    {
        utils_parse_places(src_coord, dst_coord, argv);
    }
}

void utils_get_index(long long *src_index, long long *dst_index, char *argv[], RTree *tree, Graph *g, std::vector<AdjList> adj) {
    Coordinate src_coord;
    Coordinate dst_coord;
    utils_get_coord(&src_coord, &dst_coord, argv);

    *src_index = rtree_nearest(tree, src_coord, g, adj);
    *dst_index = rtree_nearest(tree, dst_coord, g, adj);
}

static void utils_cache_path(char *out, size_t out_size, const char *bin_path) {
    const char *slash     = strrchr(bin_path, '/');
    const char *backslash = strrchr(bin_path, '\\');
    const char *filename  = bin_path;

    if (slash && (!backslash || slash > backslash)) filename = slash + 1;
    else if (backslash) filename = backslash + 1;

    snprintf(out, out_size, "data/contractions/%s", filename);
}

void utils_print_results(ResultPath *rp) {
    if (rp == NULL) {
        std::cout << "dFailed to find path" << std::endl;
    } else {
        std::cout   << rp->name << "\n"
                    << "\ttravel time:          " << rp->time_in_seconds / 60 << " minutes\n"
                    << "\tdistance travelled:   " << rp->distance_in_metres / 1000 << " kms\n"
                    << "\ttime to load          " << rp->load_time_in_seconds << " seconds" <<
        std::endl;
    }
}

LoadedVariables utils_load_variables(const char *bin_path)
{
    LoadedVariables vars{};
    clock_t t_stage = clock();

    vars.g = graph_load(bin_path);
    std::cout << "graph_load:           " << (double)(clock() - t_stage) / CLOCKS_PER_SEC << "s" << std::endl;

    t_stage = clock();
    for (long long i = 0; i < (long long)vars.g->nodes.size(); i++) {
        vars.map[vars.g->nodes[i].id] = i;
    }
    std::cout << "hashmap_create:       " << (double)(clock() - t_stage) / CLOCKS_PER_SEC << "s" << std::endl;

    char cache_path[512];
    utils_cache_path(cache_path, sizeof(cache_path), bin_path);

    vars.loaded_from_cache = false;

    t_stage = clock();
    vars.ch_g = ch_load(cache_path, vars.g, vars.adj, vars.adj_r);
    std::cout << "ch_load:              " << (double)(clock() - t_stage) / CLOCKS_PER_SEC << "s" << std::endl;

    if (vars.ch_g) {
        vars.loaded_from_cache = 1;
    } else {
        t_stage = clock();
        vars.adj   = adjlist_create(vars.g, vars.map, 0);
        vars.adj_r = adjlist_create(vars.g, vars.map, 1);
        std::cout << "adjlist_create x2:    " << (double)(clock() - t_stage) / CLOCKS_PER_SEC << "s" << std::endl << std::endl;

        vars.ch_g = ch_build(vars.g, vars.adj, vars.adj_r);   // prints its own internal timing already

        t_stage = clock();
        ch_save(cache_path, vars.g, vars.adj, vars.adj_r, vars.ch_g);
        std::cout << "ch_save:              " << (double)(clock() - t_stage) / CLOCKS_PER_SEC << "s" << std::endl << std::endl;
    }

    t_stage = clock();
    vars.tree = rtree_build(vars.g);
    std::cout << "rtree_build:          " << (double)(clock() - t_stage) / CLOCKS_PER_SEC << "s" << std::endl << std::endl;
    std::cout << "Number of nodes: " << vars.g->node_count << ", edges " << vars.g->edge_count << std::endl << std::endl;

    return vars;
}