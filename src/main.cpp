#include <iostream>
#include <graph.h>
#include "utils.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"
#include "window.h"
#include "shader.h"
#include "renderer.h"
#include "camera.h"
/*
This is the C++ rewrite of the map-pathfinding project,
this is because i was able to complete the pathfinding
logic and am now looking to create a gui and OpenGL
rendering of the path, this is quite difficult in C, so
i am deciding to rewrite it in c++, most of the structure
of the code that i wrote is already built into c so the
project will be a lot smaller
*/

// int argc, char* argv[]
int main()
{
    GLFWwindow *window = initialiseVariables(800, 600);
    if (!window) return -1;

    // compile the shader
    unsigned int vertexShader = compile_vertex_shader(); 
    unsigned int fragmentShader = compile_fragment_shader();
    unsigned int shaderProgram = create_shader_program(vertexShader, fragmentShader);

    double test_lat = 45.44594600337907;
    double test_lon = 12.322594004786044;
    
    // define camera starting pos (venice)
    Camera cam;
    cam.centre_lat = 45.44594600337907;
    cam.centre_lon = 45.312594004716040;
    cam.zoom = 3;

    // set camera
    set_camera(&cam);

    // set callback functions, this is used for input checking
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // rendering commands will go here...
        
        // add resizing, this needs to be done for the coordinate system
        int window_width;
        int window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);

        // set background colour
        glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render verticies
        glUseProgram(shaderProgram);
        WindowCoordinate ndc = latlon_to_ndc(test_lat, test_lon, cam, window_width, window_height);
        draw_square(ndc.x, ndc.y, 0.1, (float)window_width / window_height);

        // check call events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}