#pragma once
#include <vector>
#include <string>

// an enum storing all the roadtypes possible for osm
enum RoadType {
    Motorway,
    MotorwayLink,
    Trunk,
    TrunkLink,
    Primary,
    PrimaryLink,
    Secondary,
    SecondaryLink,
    Tertiary,
    TertiaryLink,
    Unclassified,
    Residential,
    LivingStreet,
    Service
};

// define the Node structure
struct Node {
    long long   id;
    double      lat;
    double      lon;
};

// define the Edge structure
struct Edge {
    long long       src;
    long long       dst;
    double          weight;
    RoadType        road_type;
    int             one_way;
    int             speed_limit;
};

// define the Graph stucture, containing all Nodes and Edges
struct Graph {
    std::vector<Node>   nodes;
    std::vector<Edge>   edges;
    long long           node_count;
    long long           edge_count;
};

struct Coordinate {
    double lat;
    double lon;
};

struct ResultPath {
    std::string name;
    std::vector<long long>  path_inx;
    double                  time_in_seconds;
    double                  distance_in_metres;
    double                  load_time_in_seconds;
};

// define functions for graph_c
Graph* graph_load(const char* path);
// void result_path_free(ResultPath *rp);
