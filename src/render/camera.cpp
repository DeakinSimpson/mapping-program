#include "camera.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"

// this converts the worlds x and y positions to the screens x and y positions
WindowCoordinate world_to_screen(WindowCoordinate world_pos, WindowCoordinate cam_world_pos, int window_width, int window_height) {
    // how far is the pixel from the centre of the camera
    double offset_x = world_pos.x - cam_world_pos.x;
    double offset_y = world_pos.y - cam_world_pos.y;

    // where does the pixel land on the window
    double screen_x = offset_x + (window_width / 2);
    double screen_y = offset_y + (window_height / 2);

    // give the screen coordinate position
    WindowCoordinate screen_coord;
    screen_coord.x = screen_x;
    screen_coord.y = screen_y;

    return screen_coord;
}

// converts to normolised device coordinated (-1<->1) on the screen
WindowCoordinate screen_to_ndc(WindowCoordinate screen_pos, int window_width, int window_height) {
    // normalise screen pos to (0-1)
    double ndc_x = screen_pos.x / window_width;
    double ndc_y = screen_pos.y / window_height;

    // normalises to -1 <-> 1
    ndc_x = ndc_x * 2.0 - 1.0;
    ndc_y = ndc_y * 2.0 - 1.0;

    // flip y, this is because the 0,0 is in the top left of the screen
    ndc_y = -ndc_y;

    double aspect = (double)window_width / window_height;

    ndc_x = ndc_x / aspect;

    WindowCoordinate ndc_coord;
    ndc_coord.x = ndc_x;
    ndc_coord.y = ndc_y;

    return ndc_coord;
}

// conveerts lat and lon variables to ndc points
WindowCoordinate latlon_to_ndc(double lat, double lon, Camera cam, int window_width, int window_height) {
    WindowCoordinate camera_world_pos;
    WindowCoordinate world_pos;
    WindowCoordinate screen_pos;
    WindowCoordinate ndc_pos;

    camera_world_pos = convert_to_coordinate(cam.centre_lat, cam.centre_lon, cam.zoom);
    world_pos = convert_to_coordinate(lat, lon, cam.zoom);
    screen_pos = world_to_screen(world_pos, camera_world_pos, window_width, window_height);
    ndc_pos = screen_to_ndc(screen_pos, window_width, window_height);

    return ndc_pos;
}

// initialises camera
Camera camera_init_null() {
    Camera cam;
    cam.centre_lat = 0.0;
    cam.centre_lon = 0.0;
    cam.zoom = 0.0;

    return cam;
}