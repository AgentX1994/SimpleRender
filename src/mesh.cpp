#include "mesh.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

bool Mesh::loadObj(const std::string& filename)
{
    std::ifstream obj_file(filename);
    if (!obj_file)
        return false;

    vertices.clear();
    indices.clear();
    model_name.clear();

    std::vector<glm::vec3> input_vertices;
    std::vector<glm::vec2> input_uvs;
    std::vector<glm::vec3> input_normals;

    std::string line;

    while (std::getline(obj_file, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream stream(line);

        std::string line_type;
        stream >> line_type;

        spdlog::info("\treading line {}", line);
        if (line_type == "v") {
            glm::vec3 vertex;
            stream >> vertex.x;
            stream >> vertex.y;
            stream >> vertex.z;
            input_vertices.push_back(vertex);
            spdlog::info("\t\tread vertex {{{},{},{}}}", vertex.x, vertex.y, vertex.z);
        } else if (line_type == "vn") {
            glm::vec3 normal;
            stream >> normal.x;
            stream >> normal.y;
            stream >> normal.z;
            input_normals.push_back(normal);
            spdlog::info("\t\tread normal {{{},{},{}}}", normal.x, normal.y, normal.z);
        } else if (line_type == "vt") {
            glm::vec2 uv;
            stream >> uv.x;
            stream >> uv.y;
            input_uvs.push_back(uv);
            spdlog::info("\t\tread uv {{{},{}}}", uv.x, uv.y);
        } else if (line_type == "f") {
            glm::uvec3 face;
            for (int i = 0; i < 3; i++) {
                size_t vertex_index, uv_index, normal_index;
                stream >> vertex_index;

                uv_index = vertex_index;
                if (stream.peek() == '/') {
                    char slash;
                    stream >> slash;
                    if (stream.peek() != '/') {
                        stream >> uv_index;
                    }
                }

                normal_index = vertex_index;
                if (stream.peek() == '/') {
                    char slash;
                    stream >> slash;
                    stream >> normal_index;
                }

                Vertex v;
                v.pos = input_vertices[vertex_index - 1];
                spdlog::info("\t\t\tvertex = {{{},{},{}}}", v.pos.x, v.pos.y, v.pos.z);
                if (input_uvs.size() > uv_index) {
                    v.uv = input_uvs[uv_index - 1];
                    spdlog::info("\t\t\tuv = {{{},{}}}", v.uv.x, v.uv.y);
                }
                if (input_normals.size() > normal_index) {
                    v.normal = input_normals[normal_index - 1];
                    spdlog::info("\t\t\tnormal = {{{},{},{}}}", v.normal.x, v.normal.y, v.normal.z);
                }
                spdlog::info("\t\tread face {}/{}/{}", vertex_index, uv_index, normal_index);

                size_t index;
                auto it = std::find(vertices.begin(), vertices.end(), v);
                if (it == vertices.end()) {
                    index = vertices.size();
                    vertices.push_back(v);
                } else {
                    index = it - vertices.begin();
                }

                face[i] = index;
            }

            indices.push_back(face);
        } else if (line_type == "g") {
            stream >> model_name;
            spdlog::info("\t\tread name \"{}\"", model_name);
        } else {
            spdlog::warn("Unknown line in {} : \"{}\"", filename, line);
        }
    }

    spdlog::info("{} loaded, model name = \"{}\", {} vertices, {} uvs, {} normals, {} faces", filename, model_name, input_vertices.size(), input_uvs.size(), input_normals.size(), indices.size());
    return true;
}
