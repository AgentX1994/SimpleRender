#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "vertex.h"

class Mesh {
public:
    Mesh() = default;

    Mesh(const Mesh&) = default;
    Mesh& operator=(const Mesh&) = default;

    Mesh(Mesh&&) = default;
    Mesh& operator=(Mesh&&) = default;

    bool loadObj(const std::string& filename);

    const std::vector<Vertex>& getVertices() { return vertices; }
    const std::vector<glm::uvec3>& getIndices() { return indices; }

private:
    std::vector<Vertex> vertices;
    std::vector<glm::uvec3> indices;
    std::string model_name;
};
