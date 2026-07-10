#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"
#include "camera.h"

// initialise the window and variables
GLFWwindow *initialiseVariables(int initial_width, int initial_height);

// process all inputs into the window
void processInput(GLFWwindow *window);

// sets the local camera variable
void set_camera(Camera *cam);