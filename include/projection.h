#pragma once

struct WindowCoordinate {
    double x;
    double y;
};

WindowCoordinate convert_to_coordinate(double lat, double lon, double zoom);