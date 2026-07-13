#include "renderer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"
#include "utils.h"
#include "projection.h"
#include "camera.h"

struct RenderEdge {
    float x1;
    float y1;
    float z1;

    float x2;
    float y2;
    float z2;
};

static unsigned int VAO; // holds setup info (including VBO & EBO)
static unsigned int VBO; // holds vertex positions
static unsigned int EBO; // holds indicies

// world map variables
static unsigned int world_map_VAO, world_map_VBO, world_map_EBO;
static int world_map_index_count = 0;

void initialise_rendering_variables() {
    // generate 1 ID and store it in the position &name 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); // bind VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // make VBO the active buffer

    /*
    0 — which attribute to configure (matches layout (location = 0) in your vertex shader)
    3 — each vertex has 3 values (x, y, z)
    GL_FLOAT — those values are floats
    GL_FALSE — don't normalise the values
    3 * sizeof(float) — the stride, how many bytes to jump to get to the next vertex
    (void*)0 — offset of where the data starts in the buffer (0 = start from the beginning)    
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);   // enables the attribute as they are disabled by default

    // unbinds VAO by binding 0
    glBindVertexArray(0);    
}

void free_rendering_variables() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &world_map_VAO);
    glDeleteBuffers(1, &world_map_VBO);
    glDeleteBuffers(1, &world_map_EBO);
}

void draw_path(const LoadedVariables &vars, const ResultPath &rp, Camera cam, int window_width, int window_height) {
    std::vector<RenderEdge> edges;

    // append all edges to the edges vector
    for (long long i = 0; i < (long long)rp.path_inx.size() - 1; i++) {
        WindowCoordinate i_ndc = latlon_to_ndc(vars.g->nodes[rp.path_inx[i]].lat, vars.g->nodes[rp.path_inx[i]].lon, cam, window_width, window_height);
        WindowCoordinate i_ndc_1 = latlon_to_ndc(vars.g->nodes[rp.path_inx[i + 1]].lat, vars.g->nodes[rp.path_inx[i + 1]].lon, cam, window_width, window_height);

        RenderEdge edge;
        edge.x1 = i_ndc.x;   edge.y1 = i_ndc.y;   edge.z1 = 0.0f;
        edge.x2 = i_ndc_1.x; edge.y2 = i_ndc_1.y; edge.z2 = 0.0f;
        edges.push_back(edge);
    }

    // send the edges data straight to the gpu same as float[]
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, edges.size() * sizeof(RenderEdge), edges.data(), GL_DYNAMIC_DRAW);

    // verticies array is edges array * 2 as there are twice as many verticies as edges
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, edges.size() * 2);
    glBindVertexArray(0);    
}

// upload the model to the gpu
void upload_model(const TriangulatedModel &model) {
    // set the current buffers to active
    glGenVertexArrays(1, &world_map_VAO);
    glGenBuffers(1, &world_map_VBO);
    glGenBuffers(1, &world_map_EBO);

    // bind VAO to buffer
    glBindVertexArray(world_map_VAO);

    // bind VBO to buffer
    glBindBuffer(GL_ARRAY_BUFFER, world_map_VBO);

    // set the current verticies to the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, model.verticies.size() * sizeof(Vertex), model.verticies.data(), GL_STATIC_DRAW);

    // bind to the EBO buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, world_map_EBO);

    // put indicies to the buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indicies.size() * sizeof(uint32_t), model.indicies.data(), GL_STATIC_DRAW);

    // draw the shape
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // clear buffers
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    world_map_index_count = (int)model.indicies.size();
}

// use the stored information in the gpu to render the map
void draw_model(unsigned int shaderProgram, Camera cam, int window_width, int window_height) {
    // set the variables in the gpu to the given variables
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "u_cam_lat"), (float)cam.centre_lat);
    glUniform1f(glGetUniformLocation(shaderProgram, "u_cam_lon"), (float)cam.centre_lon);
    glUniform1f(glGetUniformLocation(shaderProgram, "u_zoom"),    (float)cam.zoom);
    glUniform1f(glGetUniformLocation(shaderProgram, "u_width"),   (float)window_width);
    glUniform1f(glGetUniformLocation(shaderProgram, "u_height"),  (float)window_height);

    // draw elements in world_map_VAO
    glBindVertexArray(world_map_VAO);
    glDrawElements(GL_TRIANGLES, world_map_index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


