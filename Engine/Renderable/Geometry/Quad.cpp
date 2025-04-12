#include "Quad.h"

Quad::Quad()
{
    Initialize();
}

void Quad::PopulateSurfacePoints()
{
    surfacePoints.push_back(glm::vec3(-1.f, -1.f, 0.f));
    surfacePoints.push_back(glm::vec3(1.f, -1.f, 0.f));
    surfacePoints.push_back(glm::vec3(1.f, 1.f, 0.f));
    surfacePoints.push_back(glm::vec3(-1.f, 1.f, 0.f));
}

void Quad::GenerateVertices()
{
    vertices = {
        Vertex(surfacePoints[0], {1.f, 0.f, 1.f}, {0.f, 0.f}),
        Vertex(surfacePoints[1], {1.f, 0.f, 1.f}, {0.f, 0.f}),
        Vertex(surfacePoints[2], {1.f, 0.f, 1.f}, {0.f, 0.f}),
        Vertex(surfacePoints[3], {1.f, 0.f, 1.f}, {0.f, 0.f})
    };
}

void Quad::GenerateIndices()
{
    indices = {
        0, 1, 2, 2, 3, 0
    };
}