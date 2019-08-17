#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;

    bool operator==(const Vertex& other)
    {
        return pos == other.pos
            && uv == other.uv
            && normal == other.normal;
    }
};
