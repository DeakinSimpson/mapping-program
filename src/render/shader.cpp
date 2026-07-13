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

static const char *mapVertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"   // x=lon, y=lat
    "uniform float u_cam_lat;\n"
    "uniform float u_cam_lon;\n"
    "uniform float u_zoom;\n"
    "uniform float u_width;\n"
    "uniform float u_height;\n"
    "const float PI = 3.14159265;\n"
    "const float TILE_SIZE = 256.0;\n"
    "vec2 to_world(float lat, float lon) {\n"
    "    float lat_rad = lat * PI / 180.0;\n"
    "    float x = (lon + 180.0) / 360.0 * pow(2.0, u_zoom) * TILE_SIZE;\n"
    "    float y = (1.0 / (2.0 * PI)) * pow(2.0, u_zoom) * (PI - log(tan(PI/4.0 + lat_rad/2.0))) * TILE_SIZE;\n"
    "    return vec2(x, y);\n"
    "}\n"
    "void main() {\n"
    "    vec2 world     = to_world(aPos.y, aPos.x);\n"
    "    vec2 cam_world = to_world(u_cam_lat, u_cam_lon);\n"
    "    float screen_x = (world.x - cam_world.x) + u_width  / 2.0;\n"
    "    float screen_y = (world.y - cam_world.y) + u_height / 2.0;\n"
    "    float ndc_x = (screen_x / u_width  * 2.0 - 1.0) / (u_width / u_height);\n"
    "    float ndc_y = -(screen_y / u_height * 2.0 - 1.0);\n"
    "    gl_Position = vec4(ndc_x, ndc_y, 0.0, 1.0);\n"
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

unsigned int compile_map_vertex_shader() {
    unsigned int shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &mapVertexShaderSource, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::MAP_VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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