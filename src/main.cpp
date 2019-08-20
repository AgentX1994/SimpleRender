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

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <spdlog/async.h>

#include "debug_callback.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"

void error_callback(int error, const char* des)
{
    std::cerr << "Error: " << error << " (\"" << des << "\")\n";
}

bool debug_mode = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        debug_mode = !debug_mode;
        spdlog::debug("Debug mode = {}", debug_mode ? "on" : "off");
    }
}

void print_usage(std::string name)
{
    fmt::print("Usage: {} [-v[v...]] [mesh]", name);
    fmt::print("\tmultiple v's can be used in -v to increase verbosity, e.g. -vvv");
    fmt::print("\tIf no mesh is given, test.obj is used");
}

int main(int argc, char** argv)
{
    std::string mesh_file = "test.obj";
    bool mesh_file_given = false;
    int verbosity = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            std::string vs(argv[i] + 1);
            for (auto c : vs) {
                if (c == 'v') {
                    verbosity++;
                } else {
                    print_usage(argv[0]);
                    return EXIT_FAILURE;
                }
            }
        } else {
            if (mesh_file_given) {
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
            mesh_file = argv[i];
            mesh_file_given = true;
        }
    }

    spdlog::level::level_enum level;
    if (verbosity == 0) {
        level = spdlog::level::err;
    } else if (verbosity == 1) {
        level = spdlog::level::warn;
    } else if (verbosity == 2) {
        level = spdlog::level::info;
    } else if (verbosity == 3) {
        level = spdlog::level::debug;
    } else {
        level = spdlog::level::trace;
    }

    // set up logger
    {
        spdlog::init_thread_pool(8192, 1);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(level);

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("simple_render.txt", true);
        file_sink->set_level(level);

        std::vector<spdlog::sink_ptr> sinks { console_sink, file_sink };

        auto logger = std::make_shared<spdlog::async_logger>("simple_render", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
        spdlog::register_logger(logger);

        spdlog::set_default_logger(logger);
    }

    Mesh my_mesh;
    my_mesh.loadObj(mesh_file);

    if (!glfwInit()) {
        spdlog::error("Could not load glfw!");
        return EXIT_FAILURE;
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

    // Scope to prevent openGL objects from being destructed after GLFW is terminated
    {
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, key_callback);
        glfwSwapInterval(1);

        spdlog::info("OpenGL Version: {}", epoxy_gl_version());
        glDebugMessageCallback(debug_callback, nullptr);

        GLuint VAO; // Vertex Array object
        GLuint vertex_buffer; // Vertex Buffer Object
        GLuint index_buffer; // Element Buffer object

        // vertex attribute locations
        GLint vpos_location; // location of vPos attribute
        GLint vuv_location; // location of vUv attribute
        GLint vnorm_location; // location of vNorm attribute

        ShaderProgram basic_shader("shaders/basic.vert", "shaders/basic.frag");
        ShaderProgram debug_shader("shaders/debug.vert", "shaders/debug.frag");

        spdlog::trace("gen buffers");
        // Generate OpenGL buffers
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        spdlog::trace("get attribute locs");
        vpos_location = basic_shader.get_attribute_location("vPos");
        if (vpos_location == -1) {
            spdlog::warn("Could not get location of vPos");
        } else {
            spdlog::trace("add vpos");
            glEnableVertexAttribArray(vpos_location);
            spdlog::trace("set vpos data");
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        }

        vuv_location = basic_shader.get_attribute_location("vUv");
        if (vuv_location == -1) {
            spdlog::warn("Could not get location of vUv");
        } else {
            spdlog::trace("add vuv");
            glEnableVertexAttribArray(vuv_location);
            spdlog::trace("set vuv data");
            glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
        }

        vnorm_location = basic_shader.get_attribute_location("vNormal");
        if (vnorm_location == -1) {
            spdlog::warn("Could not get location of vNormal");
        } else {
            spdlog::trace("add vnorm");
            glEnableVertexAttribArray(vnorm_location);
            spdlog::trace("set vnorm data");
            glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 5));
        }

        spdlog::trace("Create index/element buffer");
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

        // Create debug VAO
        GLuint debug_VAO;
        glGenVertexArrays(1, &debug_VAO);
        glBindVertexArray(debug_VAO);

        constexpr float NORMAL_LEN = 30.0f;
        std::vector<glm::vec3> debug_vertices;
        for (auto& v : vertices) {
            spdlog::trace("vertex: {{ pos {{{: >+#8.3f},{: >+#8.3f},{: >+#8.3f}}}, uv {{{: >+#8.3f},{: >+#8.3f}}}, normal {{{: >+#8.3f},{: >+#8.3f},{: >+#8.3f}}} }}", v.pos.x, v.pos.y, v.pos.z, v.uv.s, v.uv.t, v.normal.x, v.normal.y, v.normal.z);
            auto normal = v.normal;
            auto pos = v.pos;
            auto end_pos = pos + normal * NORMAL_LEN;
            spdlog::trace("\tResulting normal line: ({: >+#8.3f},{: >+#8.3f},{: >+#8.3f}) -> ({: >+#8.3f},{: >+#8.3f},{: >+#8.3f})", pos.x, pos.y, pos.z, end_pos.x, end_pos.y, end_pos.z);
            debug_vertices.push_back(pos);
            debug_vertices.push_back(end_pos);
        }

        GLuint debug_vertex_buffer;
        glGenBuffers(1, &debug_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, debug_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * debug_vertices.size(), debug_vertices.data(), GL_STATIC_DRAW);

        vpos_location = debug_shader.get_attribute_location("vPos");
        if (vpos_location == -1) {
            spdlog::warn("Could not get location of vPos");
        } else {
            spdlog::trace("add vpos");
            glEnableVertexAttribArray(vpos_location);
            spdlog::trace("set vpos data");
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        }

        glClearColor(0.0, 0.0, 0.0, 1.0);

        glm::mat4 m, p, mvp;
        // Position the camera at (40, 30, 30) looking towards (0, 0, 0), with (0, 1, 0) up vector
        glm::mat4 view = glm::lookAt(
            glm::vec3(40.f, 30.f, 30.f), // Camera position
            glm::vec3(0.f), // looking at
            glm::vec3(0.f, 1.f, 0.f) // Up vector
        );

        // light pos is 20 units higher than camera
        glm::vec3 light_position = glm::vec3(40.f, 40.f, 30.f);

        // coefficients
        float ambient_coefficient = 1.0f;
        float diffuse_coefficient = 1.0f;
        float specular_coefficient = 1.0f;

        float shininess = 80.f; // lower = more shiny for some reason

        // colors
        glm::vec3 ambient_color = glm::vec3(0.1f, 0.1f, 0.2f);
        glm::vec3 diffuse_color = glm::vec3(0.5f, 0.5f, 0.9f);
        glm::vec3 specular_color = glm::vec3(1.f, 1.f, 1.f);

        // Determine best scaling factor for this model
        float max_len = 0.f;
        for (const auto& v : vertices) {
            auto len = glm::length(v.pos);
            if (len > max_len) {
                max_len = len;
            }
        }
        float scale = 10.f / max_len;
        spdlog::info("scaling factor: {}", scale);

        glEnable(GL_DEPTH_TEST);

        spdlog::trace("Start drawing");
        while (!glfwWindowShouldClose(window)) {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            float ratio = width / (float)height;
            float time = glfwGetTime();

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m = glm::mat4(1.0f);
            m = glm::scale(m, glm::vec3(scale));
            m = glm::rotate(m, time, glm::vec3(0.f, 1.f, 0.0f));

            p = glm::perspective(glm::radians(45.f), ratio, .1f, 100.f);

            glm::mat4 model_view = view * m;
            mvp = p * model_view;

            glm::mat4 inv_trans_model_view = glm::transpose(glm::inverse(model_view));

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            basic_shader.use();

            // Transformations
            basic_shader.set_uniform_mat4("MVP", mvp);
            basic_shader.set_uniform_mat4("model_view", model_view);
            basic_shader.set_uniform_mat4("inv_trans_model_view", inv_trans_model_view);

            // light position
            basic_shader.set_uniform_vec3("light_position", light_position);

            // coefficients
            basic_shader.set_uniform_float("ambient_coefficient", ambient_coefficient);
            basic_shader.set_uniform_float("diffuse_coefficient", diffuse_coefficient);
            basic_shader.set_uniform_float("specular_coefficient", specular_coefficient);

            basic_shader.set_uniform_float("shininess", shininess);

            // colors
            basic_shader.set_uniform_vec3("ambient_color", ambient_color);
            basic_shader.set_uniform_vec3("diffuse_color", diffuse_color);
            basic_shader.set_uniform_vec3("specular_color", specular_color);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr);

            basic_shader.unuse();
            // Also draw normals
            if (debug_mode) {

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                debug_shader.use();
                // Transformations
                debug_shader.set_uniform_mat4("MVP", mvp);

                // mode
                debug_shader.set_uniform_int("mode", 0);

                glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr);

                debug_shader.set_uniform_int("mode", 1);
                glBindVertexArray(debug_VAO);
                glDrawArrays(GL_LINES, 0, debug_vertices.size());
                debug_shader.unuse();
            }

            glBindVertexArray(0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
