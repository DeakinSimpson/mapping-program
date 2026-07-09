#include <iostream>
#include <graph.h>
#include "utils.h"
/*
This is the C++ rewrite of the map-pathfinding project,
this is because i was able to complete the pathfinding
logic and am now looking to create a gui and OpenGL
rendering of the path, this is quite difficult in C, so
i am deciding to rewrite it in c++, most of the structure
of the code that i wrote is already built into c so the
project will be a lot smaller
*/

// int main(int argc, char* argv[])
int main(int argc, char* argv[]) {
    if (argc != 7  && argc != 5) {
        std::cout << "usage: ./builder/pathfinder.exe <path_to_bin> -c <src lat> <src lon> <dst lat> <dst lon>" << std::endl;
        std::cout << "usage: ./builder/pathfinder.exe <path_to_bin> -d <src place> <dst place>" << std::endl;
        return 1;
    }

    // load in all variables
    LoadedVariables vars = utils_load_variables(argv[1]);

    long long src_index;
    long long dst_index;

    utils_get_index(&src_index, &dst_index, argv, vars.tree, vars.g, vars.adj);

    ResultPath ch_rp = ch_query(vars.g, vars.ch_g, vars.adj, vars.adj_r, vars.map, vars.g->nodes[src_index].id, vars.g->nodes[dst_index].id);
    utils_print_results(&ch_rp);

    std::cout << vars.adj[0][0].dst_index << std::endl;

    return 0;
}