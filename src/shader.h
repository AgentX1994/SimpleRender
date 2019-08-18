#pragma once

#include <string>

#include <epoxy/gl.h>

class ShaderProgram {
public:
    ShaderProgram(
        std::string vertex_file,
        std::string fragment_file,
        std::string geometry_file = "",
        std::string tesselation_control_file = "",
        std::string tesselation_evaluation_file = "");

    void use();
    void unuse();

    GLint get_uniform_location(const std::string& uniform);
    GLint get_attribute_location(const std::string& attribute);

private:
    GLuint create_shader(std::string file, GLenum type);

    std::string vertex_file;
    std::string fragment_file;
    std::string geometry_file;
    std::string tesselation_control_file;
    std::string tesselation_evaluation_file;

    GLuint shader_program_id;
};
