#include "window.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"
#include "iostream"
#include "math.h"

// set max and min lat values to stop values going to infinity at the edges
static const double lat_lim = 85.0511;

// initialises a camera variable
static Camera *g_cam = nullptr;

// sets mouse states
static bool  g_dragging  = false;
static double g_last_x   = 0.0;
static double g_last_y   = 0.0;

// forward declerations for initalise variables
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
static void cursor_pos_callback(GLFWwindow *window, double x, double y);

GLFWwindow *initialiseVariables(int initial_width, int initial_height) {
    // initialise the window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a window object
    GLFWwindow *window = glfwCreateWindow(800, 600, "Map", NULL, NULL);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // initialise GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to Initialise GLAD" << std::endl;
    }

    glViewport(0, 0, 800, 600);

    // set the callback functions to the custome ones bellow
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    return window;
}

// sets the viewport width and height to the windows width and height, allowing resizing
static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// this processes the individual inputs from the keyboard or mouse
void processInput(GLFWwindow *window) {
    // processes ESCAPE input and closes window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// sets the local g_cam variable
void set_camera(Camera *cam) {
    g_cam = cam;
}

// All callback functions for movement
// --------------------------------------------------------------------------------------------------

// lets the scroll function change the g_cam zoom level
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (g_cam) g_cam->zoom += yoffset;
}

// this registes mousebutton clicks
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    // if mouse button pressed then set state to dragging start moving screen
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        g_dragging = true;
        glfwGetCursorPos(window, &g_last_x, &g_last_y);
    }

    // if the mouse is release then stop dragging
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        g_dragging = false;
    }
}

// moves the g_cam as the change in x and y to move its centre pos
static void cursor_pos_callback(GLFWwindow *window, double x, double y) {
    if (!g_dragging || !g_cam) return;

    double dx = x - g_last_x;
    double dy = y - g_last_y;
    g_last_x = x;
    g_last_y = y;

    double scale = 360.0 / (256.0 * pow(2.0, g_cam->zoom));
    g_cam->centre_lon -= dx * scale;
    g_cam->centre_lat += dy * scale;

    // clamps the top and bottom of the world
    if (g_cam->centre_lat > lat_lim) g_cam->centre_lat = lat_lim;
    if (g_cam->centre_lat < -lat_lim) g_cam->centre_lat = -lat_lim;

    // makes the map wrap around on left and right (lon)
    double wrapped_lon = fmod(g_cam->centre_lon + 180.0, 360.0);    // add 180 and mod by 360, this is to get a mod from [-180, 180] instead of [0, 360]
    if (wrapped_lon < 0) wrapped_lon += 360;                        // c++ can return negative numbers for fmod, so add 360 to keep positive
    g_cam->centre_lon = wrapped_lon - 180;                          // +180 to set back to coordinates used in the viewport
}