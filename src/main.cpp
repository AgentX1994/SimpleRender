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
#include "shader.h"
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

void print_usage(std::string name)
{
    fmt::print("Usage: {} [mesh]", name);
    fmt::print("\tIf no mesh is given, test.obj is used");
}

int main(int argc, char** argv)
{
    std::string mesh_file = "test.obj";
    if (argc == 2) {
        mesh_file = argv[1];
    } else if (argc > 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
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

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    spdlog::info("OpenGL Version: {}", epoxy_gl_version());
    glDebugMessageCallback(debug_callback, nullptr);

    GLuint VAO; // Vertex Array object
    GLuint vertex_buffer; // Vertex Buffer Object
    GLuint index_buffer; // Element Buffer object

    // uniform locations
    GLint mvp_location; // location of MVP uniform
    GLint inv_trans_model_view_location; // location of inv_trans_model_view uniform
    GLint model_view_location; // location of model_view uniform
    GLint light_position_location; // location of light position uniform

    GLint ambient_coefficient_location; // location of ambient coefficient uniform
    GLint diffuse_coefficient_location; // location of diffuse coefficient uniform
    GLint specular_coefficient_location; // location of specular coefficient uniform

    GLint shininess_location; // location of shininess uniform

    GLint ambient_color_location; // location of ambient color uniform
    GLint diffuse_color_location; // location of diffuse color uniform
    GLint specular_color_location; // location of specular color uniform

    // vertex attribute locations
    GLint vpos_location; // location of vPos attribute
    GLint vuv_location; // location of vUv attribute
    GLint vnorm_location; // location of vNorm attribute

    ShaderProgram basic_shader("shaders/basic.vert", "shaders/basic.frag");

    spdlog::info("gen buffers");
    // Generate OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    spdlog::info("get uniform locs");
    mvp_location = basic_shader.get_uniform_location("MVP");
    if (mvp_location == -1) {
        spdlog::warn("Could not get location of MVP");
    }

    inv_trans_model_view_location = basic_shader.get_uniform_location("inv_trans_model_view");
    if (inv_trans_model_view_location == -1) {
        spdlog::warn("Could not get location of inv_trans_model_view");
    }

    model_view_location = basic_shader.get_uniform_location("model_view");
    if (model_view_location == -1) {
        spdlog::warn("Could not get location of model_view");
    }

    light_position_location = basic_shader.get_uniform_location("light_position");
    if (light_position_location == -1) {
        spdlog::warn("Could not get location of light position");
    }

    ambient_coefficient_location = basic_shader.get_uniform_location("ambient_coefficient");
    if (ambient_coefficient_location == -1) {
        spdlog::warn("Could not get location of ambient coefficient");
    }

    diffuse_coefficient_location = basic_shader.get_uniform_location("diffuse_coefficient");
    if (diffuse_coefficient_location == -1) {
        spdlog::warn("Could not get location of diffuse coefficient");
    }

    specular_coefficient_location = basic_shader.get_uniform_location("specular_coefficient");
    if (specular_coefficient_location == -1) {
        spdlog::warn("Could not get location of specular coefficient");
    }

    shininess_location = basic_shader.get_uniform_location("shininess");
    if (shininess_location == -1) {
        spdlog::warn("Could not get location of shininess");
    }

    ambient_color_location = basic_shader.get_uniform_location("ambient_color");
    if (ambient_color_location == -1) {
        spdlog::warn("Could not get location of ambient color");
    }

    diffuse_color_location = basic_shader.get_uniform_location("diffuse_color");
    if (diffuse_color_location == -1) {
        spdlog::warn("Could not get location of diffuse color");
    }

    specular_color_location = basic_shader.get_uniform_location("specular_color");
    if (specular_color_location == -1) {
        spdlog::warn("Could not get location of specular color");
    }

    spdlog::info("get attribute locs");
    vpos_location = basic_shader.get_attribute_location("vPos");
    if (vpos_location == -1) {
        spdlog::warn("Could not get location of vPos");
    } else {
        spdlog::info("add vpos");
        glEnableVertexAttribArray(vpos_location);
        spdlog::info("set vpos data");
        glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    }

    vuv_location = basic_shader.get_attribute_location("vUv");
    if (vuv_location == -1) {
        spdlog::warn("Could not get location of vUv");
    } else {
        spdlog::info("add vuv");
        glEnableVertexAttribArray(vuv_location);
        spdlog::info("set vuv data");
        glVertexAttribPointer(vuv_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
    }

    vnorm_location = basic_shader.get_attribute_location("vNormal");
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
        m = glm::scale(m, glm::vec3(scale));
        m = glm::rotate(m, time, glm::vec3(0.f, 1.f, 0.0f));

        p = glm::perspective(glm::radians(45.f), ratio, .1f, 100.f);

        glm::mat4 model_view = view * m;
        mvp = p * model_view;

        glm::mat4 inv_trans_model_view = glm::inverse(glm::transpose(model_view));
        basic_shader.use();

        // Transformations
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
        glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (const GLfloat*)&model_view[0][0]);
        glUniformMatrix4fv(inv_trans_model_view_location, 1, GL_FALSE, (const GLfloat*)&inv_trans_model_view[0][0]);

        // light position
        glUniform3fv(light_position_location, 1, (const GLfloat*)&light_position[0]);

        // coefficients
        glUniform1fv(ambient_coefficient_location, 1, (const GLfloat*)&ambient_coefficient);
        glUniform1fv(diffuse_coefficient_location, 1, (const GLfloat*)&diffuse_coefficient);
        glUniform1fv(specular_coefficient_location, 1, (const GLfloat*)&specular_coefficient);

        glUniform1fv(shininess_location, 1, (const GLfloat*)&shininess);

        // colors
        glUniform3fv(ambient_color_location, 1, (const GLfloat*)&ambient_color[0]);
        glUniform3fv(diffuse_color_location, 1, (const GLfloat*)&diffuse_color[0]);
        glUniform3fv(specular_color_location, 1, (const GLfloat*)&specular_color[0]);

        glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
