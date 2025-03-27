#include "Quad.h"

Quad::Quad()
{
    Initialize();
}

void Quad::GenerateSurfacePoints()
{
}

void Quad::GenerateVertices()
{
    vertices = {
        Vertex(glm::vec3(-1.f, -1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f}),
        Vertex(glm::vec3(1.f, -1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f}),
        Vertex(glm::vec3(1.f, 1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f}),
        Vertex(glm::vec3(-1.f, 1.f, 0.f), {1.f, 0.f, 1.f}, {0.f, 0.f})
    };
}

void Quad::GenerateIndices()
{
    indices = {
        0, 1, 2, 2, 3, 0
    };
}