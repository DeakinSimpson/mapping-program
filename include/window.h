#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"

// initialise the window and variables
GLFWwindow *initialiseVariables(int initial_width, int initial_height);

// process all inputs into the window
void processInput(GLFWwindow *window);

// allows the window to respond to resizing
void framebuffer_size_callback(GLFWwindow *window, int width, int height);