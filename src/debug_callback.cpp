#include "debug_callback.h"
#include <iostream>

#include <spdlog/spdlog.h>

void APIENTRY debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* /*userParam*/)
{
    std::string str_source;
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        str_source = "GL_DEBUG_SOURCE_API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        str_source = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        str_source = "GL_DEBUG_SOURCE_SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        str_source = "GL_DEBUG_SOURCE_THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        str_source = "GL_DEBUG_SOURCE_APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        str_source = "GL_DEBUG_SOURCE_OTHER";
        break;
    }
    std::string str_type;
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        str_type = "GL_DEBUG_TYPE_ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        str_type = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        str_type = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        str_type = "GL_DEBUG_TYPE_PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        str_type = "GL_DEBUG_TYPE_PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_MARKER:
        str_type = "GL_DEBUG_TYPE_MARKER";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        str_type = "GL_DEBUG_TYPE_PUSH_GROUP";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        str_type = "GL_DEBUG_TYPE_POP_GROUP";
        break;
    case GL_DEBUG_TYPE_OTHER:
        str_type = "GL_DEBUG_TYPE_OTHER";
        break;
    }
    spdlog::level::level_enum level;
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        level = spdlog::level::err;
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        level = spdlog::level::warn;
        break;
    case GL_DEBUG_SEVERITY_LOW:
        level = spdlog::level::debug;
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        level = spdlog::level::info;
        break;
    }
    std::string debug_message(message, length);
    spdlog::log(level, "GL Debug from \"{}\" -- message type \"{}\" -- id = {} -- message: \"{}\"", str_source, str_type, id, debug_message);

    std::cerr << debug_message << "\"\n";
}
