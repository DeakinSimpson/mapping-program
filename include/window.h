#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"
#include "camera.h"

// initialise the window and variables
GLFWwindow *initialiseVariables(int initial_width, int initial_height);

// process all inputs into the window
void processInput(GLFWwindow *window);

// allows the window to respond to resizing
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// implements scroll that changes zoom
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// sets the local camera variable
void set_camera(Camera *cam);

// implement movement function
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow *window, double x, double y);