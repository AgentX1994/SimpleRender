#include "shader.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

#include "utils.h"

ShaderProgram::ShaderProgram(
    std::string vertex_file,
    std::string fragment_file,
    std::string geometry_file,
    std::string tesselation_control_file,
    std::string tesselation_evaluation_file)
    : vertex_file(vertex_file)
    , fragment_file(fragment_file)
    , geometry_file(geometry_file)
    , tesselation_control_file(tesselation_control_file)
    , tesselation_evaluation_file(tesselation_evaluation_file)
    , shader_program_id(0)
{
    auto vert = create_shader(vertex_file, GL_VERTEX_SHADER);
    auto frag = create_shader(fragment_file, GL_FRAGMENT_SHADER);
    auto geo = create_shader(geometry_file, GL_GEOMETRY_SHADER);
    auto tess_cont = create_shader(tesselation_control_file, GL_TESS_CONTROL_SHADER);
    auto tess_eval = create_shader(tesselation_evaluation_file, GL_TESS_EVALUATION_SHADER);

    // Create shader program
    if (vert == 0 || frag == 0) {
        spdlog::error("Could not create at least the vertex or fragment shaders!");
        return;
    }
    spdlog::info("gen shader program");
    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vert);
    glAttachShader(shader_program_id, frag);

    if (geo != 0) {
        glAttachShader(shader_program_id, geo);
    }

    if (tess_cont != 0 && tess_eval != 0) {
        glAttachShader(shader_program_id, tess_cont);
        glAttachShader(shader_program_id, tess_eval);
    }

    glLinkProgram(shader_program_id);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(shader_program_id);
    shader_program_id = 0;
}

GLuint ShaderProgram::create_shader(std::string file, GLenum type)
{
    if (file == "") {
        // Don't bother trying to read an empty string file
        return 0;
    }

    spdlog::info("read \"{}\" shader", file);
    auto shader_opt = read_file(file);
    if (!shader_opt) {
        spdlog::error("Could not read \"{}\"!", file);
        return 0;
    }

    const std::string shader_str = *shader_opt;
    auto shader = glCreateShader(type);
    auto shader_text = shader_str.c_str();
    glShaderSource(shader, 1, &shader_text, NULL);
    glCompileShader(shader);

    spdlog::info("Error check \"{}\" shader compilation", file);

    {
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::string errorLog(maxLength, '\0');
            glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog.data());

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(shader); // Don't leak the shader.

            spdlog::error("Shader compilation failed: \"{}\"", errorLog);
            return 0;
        }
    }

    return shader;
}

bool ShaderProgram::set_uniform_int(const std::string& uniform, int value)
{
    auto loc = glGetUniformLocation(shader_program_id, uniform.c_str());
    if (loc == -1) {
        //spdlog::info("ShaderProgram {}, {} has no uniform {}", vertex_file, fragment_file, uniform);
        return false;
    }
    glUniform1iv(loc, 1, &value);
    return true;
}

bool ShaderProgram::set_uniform_float(const std::string& uniform, float value)
{
    auto loc = glGetUniformLocation(shader_program_id, uniform.c_str());
    if (loc == -1) {
        //spdlog::info("ShaderProgram {}, {} has no uniform {}", vertex_file, fragment_file, uniform);
        return false;
    }
    glUniform1fv(loc, 1, &value);
    return true;
}

bool ShaderProgram::set_uniform_vec3(const std::string& uniform, const glm::vec3& value)
{
    auto loc = glGetUniformLocation(shader_program_id, uniform.c_str());
    if (loc == -1) {
        //spdlog::info("ShaderProgram {}, {} has no uniform {}", vertex_file, fragment_file, uniform);
        return false;
    }
    glUniform3fv(loc, 1, glm::value_ptr(value));
    return true;
}

bool ShaderProgram::set_uniform_mat4(const std::string& uniform, const glm::mat4& value)
{
    auto loc = glGetUniformLocation(shader_program_id, uniform.c_str());
    if (loc == -1) {
        //spdlog::info("ShaderProgram {}, {} has no uniform {}", vertex_file, fragment_file, uniform);
        return false;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    return true;
}

GLint ShaderProgram::get_attribute_location(const std::string& attribute)
{
    return glGetAttribLocation(shader_program_id, attribute.c_str());
}

void ShaderProgram::use()
{
    glUseProgram(shader_program_id);
}

void ShaderProgram::unuse()
{
    glUseProgram(0);
}
