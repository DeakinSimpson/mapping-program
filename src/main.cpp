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

int main(int argc, char* argv[])
{
    // ----- Pathfinding section -----
    if (argc != 7  && argc != 5) {
        std::cout << "usage: ./builder/pathfinder.exe <path_to_bin> -c <src lat> <src lon> <dst lat> <dst lon>" << std::endl;
        std::cout << "usage: ./builder/pathfinder.exe <path_to_bin> -d <src place> <dst place>" << std::endl;
        return 1;
    }

    // load in all variables
    LoadedVariables vars = utils_load_variables(argv[1]);    

    long long src_index;
    long long dst_index;

    utils_get_index(&src_index, &dst_index, argv, vars.tree, vars.g, vars.adj);

    ResultPath ch_rp = ch_query(vars.g, vars.ch_g, vars.adj, vars.adj_r, vars.map, vars.g->nodes[src_index].id, vars.g->nodes[dst_index].id);
    utils_print_results(&ch_rp);

    // ----- OPENGL rendering section -----
    GLFWwindow *window = initialiseVariables(800, 600);
    if (!window) return -1;

    // compile the shader
    unsigned int vertexShader = compile_vertex_shader(); 
    unsigned int fragmentShader = compile_fragment_shader();
    unsigned int shaderProgram = create_shader_program(vertexShader, fragmentShader);

    double test_lat = 0;
    double test_lon = 0;

    // define camera starting pos (venice)
    Camera cam = camera_init_null();

    // set camera
    set_camera(&cam);

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: "  << glGetString(GL_VERSION)  << std::endl;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // rendering commands will go here...
        
        // add resizing, this needs to be done for the coordinate system
        int window_width;
        int window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        float aspect = (float)window_width / window_height;

        // set background colour
        glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render verticies
        glUseProgram(shaderProgram);
        WindowCoordinate ndc = latlon_to_ndc(test_lat, test_lon, cam, window_width, window_height);
        draw_square(ndc.x, ndc.y, 0.01, aspect);

        draw_path(vars, ch_rp, cam, window_width, window_height);

        // check call events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteProgram(shaderProgram);
    free_rendering_variables();

    glfwTerminate();
    return 0;
}