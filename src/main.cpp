#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <epoxy/gl.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>

#include <spdlog/spdlog.h>

#include <spdlog/async.h>

#include "debug_callback.h"
#include "mesh.h"
#include "utils.h"

void error_callback(int error, const char* des)
{
    std::cerr << "Error: " << error << " (\"" << des << "\")\n";
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main()
{
    Mesh my_mesh;
    my_mesh.loadObj("test.obj");

    if (!glfwInit()) {
        spdlog::error("Could not load glfw!");
        return 1;
    }

    std::vector<Vertex> vertices = my_mesh.getVertices();
    std::vector<glm::uvec3> indices = my_mesh.getIndices();

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "OpenGL Test", NULL, NULL);
    if (!window) {
        spdlog::error("Could not create window!");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    spdlog::info("OpenGL Version: {}", epoxy_gl_version());
    glDebugMessageCallback(debug_callback, nullptr);

    GLuint VAO;
    GLuint vertex_buffer, index_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vuv_location, vnorm_location;

    spdlog::info("read v shader");
    auto vertex_shader_opt = read_file("shaders/basic.vert");
    if (!vertex_shader_opt) {
        spdlog::error("Could not read vertex shader!");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    const std::string vertex_shader_str = *vertex_shader_opt;

    spdlog::info("read f shader");
    auto fragment_shader_opt = read_file("shaders/basic.frag");
    if (!fragment_shader_opt) {
        spdlog::error("Could not read frag shader!");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    const std::string fragment_shader_str = *fragment_shader_opt;

    spdlog::info("gen v shader");
    // Create vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto vertex_shader_text = vertex_shader_str.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    spdlog::info("Error check v shader compilation");

    {
        GLint isCompiled = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::string errorLog(maxLength, '\0');
            glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, errorLog.data());

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(vertex_shader); // Don't leak the shader.

            spdlog::error("Vertex Shader compilation failed: \"{}\"", errorLog);

            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }
    }

    spdlog::info("gen f shader");
    // Create fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragment_shader_text = fragment_shader_str.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    spdlog::info("Error check f shader compilation");

    {
        GLint isCompiled = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::string errorLog(maxLength, '\0');
            glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, errorLog.data());

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(vertex_shader); // Don't leak the shader.
            glDeleteShader(fragment_shader); // Don't leak the shader.

            spdlog::error("Fragment Shader compilation failed: \"{}\"", errorLog);

            glfwDestroyWindow(window);
            glfwTerminate();
            return EXIT_FAILURE;
        }
    }

    spdlog::info("gen shader program");
    // Create shader program
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    spdlog::info("del shader program");
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    spdlog::info("gen buffers");
    // Generate OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    spdlog::info("get uniform locs");
    mvp_location = glGetUniformLocation(program, "MVP");
    if (mvp_location == -1) {
        spdlog::warn("Could not get location of MVP");
    }

    vpos_location = glGetAttribLocation(program, "vPos");
    if (vpos_location == -1) {
        spdlog::warn("Could not get location of vPos");
    } else {
        spdlog::info("add vpos");
        glEnableVertexAttribArray(vpos_location);
        spdlog::info("set vpos data");
        glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    }

    vuv_location = glGetAttribLocation(program, "vUv");
    if (vuv_location == -1) {
        spdlog::warn("Could not get location of vUv");
    } else {
        spdlog::info("add vuv");
        glEnableVertexAttribArray(vuv_location);
        spdlog::info("set vuv data");
        glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
    }

    vnorm_location = glGetAttribLocation(program, "vNormal");
    if (vnorm_location == -1) {
        spdlog::warn("Could not get location of vNormal");
    } else {
        spdlog::info("add vnorm");
        glEnableVertexAttribArray(vnorm_location);
        spdlog::info("set vnorm data");
        glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 5));
    }

    spdlog::info("Create index/element buffer");
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glm::mat4 m, p, mvp;
    glm::mat4 view = glm::lookAt(glm::vec3(40.f, 30.f, 30.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    glEnable(GL_DEPTH_TEST);

    spdlog::info("Start drawing");
    float last_time = glfwGetTime();
    float time_until_next_print = 1.0f;
    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float ratio = width / (float)height;
        float time = glfwGetTime();
        float elapsed_time = time - last_time;
        last_time = time;
        time_until_next_print -= elapsed_time;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m = glm::mat4(1.0f);
        m = glm::scale(m, glm::vec3(0.1f));
        m = glm::rotate(m, time, glm::vec3(0.f, 1.f, 0.0f));

        p = glm::perspective(glm::radians(45.f), ratio, .1f, 100.f);

        mvp = p * view * m;

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
        glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
