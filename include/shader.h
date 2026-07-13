#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"

/*
This converts the vertex points into screen positions on the window
*/
unsigned int compile_vertex_shader();

/*
This converts each mixel on the screen to a colour
*/
unsigned int compile_fragment_shader();

/*
This compiles the other shaders and sends them to the GPU to display
*/
unsigned int create_shader_program(unsigned int vertex_shader, unsigned int fragment_shader);

// perform mercader projection math on gpu instead of the cpu
unsigned int compile_map_vertex_shader();