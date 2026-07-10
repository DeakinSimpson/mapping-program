#include "projection.h"

// for PI
#define _USE_MATH_DEFINES
#include <math.h>

const double TILE_SIZE = 256.0;

WindowCoordinate convert_to_coordinate(double lat, double lon, double zoom) {
    double x;
    double y;
    WindowCoordinate coord;

    double lat_rad = lat * M_PI / 180.0;

    x = (lon + 180.0) / 360.0 * pow(2, zoom) * TILE_SIZE;
    y = (1.0 / (2.0 * M_PI)) * pow(2, zoom) * (M_PI - log(tan(M_PI/4.0 + lat_rad / 2.0))) * TILE_SIZE;

    
    coord.x = x;
    coord.y = y;

    return coord;
}