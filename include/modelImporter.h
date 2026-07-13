#pragma once

#include <vector>
#include <string>
#include <cstdint>

struct Vertex {
    float x;
    float y;
    float z;
};

struct Polygon {
    std::vector<Vertex> verticies;
};

struct Model {
    std::vector<Polygon> polygons;
};

// creates an array of verticies and indicies ready to be sent to the gpu for rendering
struct TriangulatedModel {
    std::vector<Vertex> verticies;
    std::vector<uint32_t> indicies;
};

Model load_land_polygons(const char *modelFilepath);
TriangulatedModel triangulate_model(const Model &raw_polygons);