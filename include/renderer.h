#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"
#include "utils.h"
#include "projection.h"
#include "camera.h"

void draw_square(float x, float y, float size, float aspect);
void draw_line(float x1, float y1, float x2, float y2);
void draw_path(const LoadedVariables &vars, const ResultPath &rp, Camera cam, int window_width, int window_height);
void initialise_rendering_variables();
void free_rendering_variables();
