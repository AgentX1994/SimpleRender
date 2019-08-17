#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;

    Vertex()
        : pos(0.f)
        , uv(0.f)
        , normal(0.f)
    {
    }

    Vertex(const Vertex&) = default;
    Vertex& operator=(const Vertex&) = default;

    Vertex(Vertex&&) = default;
    Vertex& operator=(Vertex&&) = default;

    bool operator==(const Vertex& other)
    {
        return pos == other.pos
            && uv == other.uv
            && normal == other.normal;
    }
};
