#include <iostream>
#include <graph.h>
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
    if (argc != 0) {
        std::cout << argc << std::endl;
    }

    std::cout << "hello world" << std::endl;

    Graph *g = graph_load(argv[1]);

    std::cout << "Nodes Loaded: " << g->node_count << std::endl;

    return 0;
}