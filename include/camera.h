#pragma once

#include "projection.h"

struct Camera {
    double centre_lat;
    double centre_lon;
    double zoom;
};

WindowCoordinate world_to_screen(WindowCoordinate world_pos, Camera cam, int window_width, int window_height);
WindowCoordinate screen_to_ndc(WindowCoordinate screen_pos, int window_width, int window_height);
WindowCoordinate latlon_to_ndc(double lat, double lon, Camera cam, int window_width, int window_height);

// initalises camera over NULL island
Camera camera_init_null();