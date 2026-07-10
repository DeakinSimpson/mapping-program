#include "shader.h"
#include <iostream>

static const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"

    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

static const char *fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

unsigned int compile_vertex_shader() {
    // create the shader object
    unsigned int shader;
    shader = glCreateShader(GL_VERTEX_SHADER);

    // attach the source code to the shader object and compile
    glShaderSource(shader, 1, &vertexShaderSource, NULL);
    glCompileShader(shader);

    // get the info of if the shader compiled
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    // print if the shader failed to compile
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int compile_fragment_shader() {
    unsigned int shader;
    shader =  glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(shader, 1, &fragmentShaderSource, NULL);
    glCompileShader(shader);

    // get the info of if the shader compiled
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    // print if the shader failed to compile
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int create_shader_program(unsigned int vertex_shader, unsigned int fragment_shader) {
// combine all the shaders into a shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertex_shader);
    glAttachShader(shaderProgram, fragment_shader);
    glLinkProgram(shaderProgram);

    int program_success;
    char program_infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &program_success);

    if (!program_success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, program_infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << program_infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return shaderProgram;
}