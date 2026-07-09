#pragma once
#include <vector>
#include <string>
#include <unordered_map>

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

struct AdjEdge {
    long long   dst_index;
    double      weight;
    double      km_weight;
    int         speed_limit;
    RoadType    road_type;    
};

using AdjList = std::vector<AdjEdge>;

// define functions for graph_c
Graph* graph_load(const char* path);
std::vector<AdjList> adjlist_create(Graph *g, std::unordered_map<long long, long long> &map, int reverse);
void adjlist_add_edge(std::vector<AdjList> &adj, long long src, long long dst, double weight, double km_weight, int speed_limit, RoadType road_type);
