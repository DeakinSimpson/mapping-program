#include "window.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"
#include "iostream"
#include "math.h"

GLFWwindow *initialiseVariables(int initial_width, int initial_height) {
    // initialise the window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to Initialise GLAD" << std::endl;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    // processes ESCAPE input and closes window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// initialises a camera variable
static Camera *g_cam = nullptr;

// lets teh scroll function change the g_cam zoom level
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (g_cam) g_cam->zoom += yoffset;
}

// sets the local g_cam variable
void set_camera(Camera *cam) {
    g_cam = cam;
}

// sets mouse states
static bool  g_dragging  = false;
static double g_last_x   = 0.0;
static double g_last_y   = 0.0;

// this registes mousebutton clicks
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
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
void cursor_pos_callback(GLFWwindow *window, double x, double y) {
    if (!g_dragging || !g_cam) return;

    double dx = x - g_last_x;
    double dy = y - g_last_y;
    g_last_x = x;
    g_last_y = y;

    double scale = 360.0 / (256.0 * pow(2.0, g_cam->zoom));
    g_cam->centre_lon -= dx * scale;
    g_cam->centre_lat += dy * scale;
}