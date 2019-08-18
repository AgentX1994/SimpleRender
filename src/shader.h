#pragma once

#include <string>

#include <epoxy/gl.h>
#include <glm/glm.hpp>

class ShaderProgram {
public:
    ShaderProgram(
        std::string vertex_file,
        std::string fragment_file,
        std::string geometry_file = "",
        std::string tesselation_control_file = "",
        std::string tesselation_evaluation_file = "");

    ~ShaderProgram();

    void use();
    void unuse();

    bool set_uniform_int(const std::string& uniform, int value);
    bool set_uniform_float(const std::string& uniform, float value);
    bool set_uniform_vec3(const std::string& uniform, const glm::vec3& value);
    bool set_uniform_mat4(const std::string& uniform, const glm::mat4& value);

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
