#include "renderer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "KHR/khrplatform.h"

void draw_square(float x, float y, float size, float aspect) {
    // define squares verticies
    float verticies[] = {
        x + size / aspect, y + size, 0.0f,  // top right    (0)
        x + size / aspect, y - size, 0.0f,  // bottom right  (1)
        x - size / aspect, y - size, 0.0f,  // bottom left   (2)
        x - size / aspect, y + size, 0.0f   // top left      (3)
    };

    // defines where to draw the lines, GPU's can only draw triangles
    unsigned int indicies[] = {
        0, 1, 3, // top right, bottom right, top left (triangle)
        1, 2, 3  // bottom right, top left, bottom left (triangle)
    };

    unsigned int VAO; // holds setup info (including VBO & EBO)
    unsigned int VBO; // holds vertex positions
    unsigned int EBO; // holds indicies

    // generate 1 ID and store it in the position &name 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); // bind VAO

    // make VBO the active buffer, then bind verticies to the active buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    
    // makes EBo the active element array buffer, then binds the indicies to it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

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

    // unbinds the VBO by binding 0
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    /*
    GL_TRIANGLES — draw triangles
    6 — draw 6 indices (2 triangles × 3 vertices each)
    GL_UNSIGNED_INT — the indices are unsigned int
    0 — start from the beginning of the EBO    
    */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    // unbinds VAO by binding 0
    glBindVertexArray(0);

    // cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}