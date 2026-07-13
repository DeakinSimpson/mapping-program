#define _USE_MATH_DEFINES
#include "modelImporter.h"
#include "shapelib/shapefil.h"
#include "earcut.hpp"
#include <array>
#include <cstdint>
#include <cmath>

static const double PI = 3.14159265358979323846;

// for M_PI
#define _USE_MATH_DEFINES
#include <math.h>

Model load_land_polygons(const char *modelFilepath, bool mercator) {
    Model land_polygons;

    // open the shapefile
    SHPHandle shp = SHPOpen(modelFilepath, "rb");


    int number_of_polygons = 0, polygon_type = 0;
    double min_bound[4], max_bound[4];

    // get info about the shape (polygon) in the file
    SHPGetInfo(shp, &number_of_polygons, &polygon_type, min_bound, max_bound);

    // loop through each polygon
    for (int i = 0; i < number_of_polygons; ++i) {
    SHPObject *shp_polygon = SHPReadObject(shp, i);

    // each shape can have multiple parts (e.g. mainland + islands)
    // treat each part as its own polygon so earcut doesn't connect them
    for (int part = 0; part < shp_polygon->nParts; part++) {
        Polygon polygon;

        int start = shp_polygon->panPartStart[part];
        int end;
        if (part + 1 < shp_polygon->nParts) {
            // not the last part, so end at the start of the next part
            end = shp_polygon->panPartStart[part + 1];
        } else {
            // last part, so end at the total vertex count
            end = shp_polygon->nVertices;
        }

        for (int j = start; j < end; j++) {
            Vertex vertex;

            // perform mercator projection map if shape is mercator coords
            if (mercator) {
                vertex.x = (float)(shp_polygon->padfX[j] / 20037508.342789244 * 180.0);
                double lat_rad = atan(exp(shp_polygon->padfY[j] / 20037508.342789244 * PI));
                vertex.y = (float)(lat_rad * 360.0 / PI - 90.0);
            } else {
                vertex.x = (float)shp_polygon->padfX[j];
                vertex.y = (float)shp_polygon->padfY[j];
            }
            vertex.z = 0.0f;

            polygon.verticies.push_back(vertex);
        }

        land_polygons.polygons.push_back(polygon);
    }

    SHPDestroyObject(shp_polygon);
}

    // close the open shape and return the polygons
    SHPClose(shp);
    return land_polygons;
}

TriangulatedModel triangulate_model(const Model &raw_polygons) {
    // create empty triangulated model
    TriangulatedModel triangulated_model;

    // loop through each polugon in the model
    for (const Polygon &current_polygon : raw_polygons.polygons) {
        // reset earcut input for each polygon
        std::vector<std::vector<std::array<float, 2>>> earcut_input;
        std::vector<std::array<float, 2>> outer_ring;


        // for each vertex in the current polygon
        for (const Vertex &v : current_polygon.verticies) {
            // push back into the outer ring each verticie
            outer_ring.push_back(std::array<float, 2>{v.x, v.y});
        }

        // push the outer ring to theearcut_input
        earcut_input.push_back(outer_ring);

        // run earcut, making sure it returns unsigned int
        std::vector<uint32_t> indicies = mapbox::earcut<uint32_t>(earcut_input);

        // offset the indicies by the current vertex count, each loop the indicie start back at 0, however it needs to be the current polygon
        uint32_t offset = triangulated_model.verticies.size();

        for (uint32_t idx : indicies) {
            triangulated_model.indicies.push_back(idx + offset);
        }

        // push verticies
        for (const Vertex &v : current_polygon.verticies) {
            triangulated_model.verticies.push_back(v);
        }
    }

    return triangulated_model;
}