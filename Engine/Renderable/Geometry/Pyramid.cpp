#include "Pyramid.h"

Pyramid::Pyramid(float baseSize, float height)
    : baseSize(baseSize), height(height)
{
    Initialize();
}

void Pyramid::GenerateSurfacePoints()
{
    float halfSize = baseSize * 0.5f;
    surfacePoints.push_back({ -halfSize, 0.0f, -halfSize }); // 0: Bottom-left
    surfacePoints.push_back({ halfSize, 0.0f, -halfSize }); // 1: Bottom-right
    surfacePoints.push_back({ halfSize, 0.0f,  halfSize }); // 2: Top-right
    surfacePoints.push_back({ -halfSize, 0.0f,  halfSize }); // 3: Top-left
    surfacePoints.push_back({ 0.0f, height, 0.0f }); // 4: Apex
    surfacePoints.push_back({ 0.0f, 0.0f, 0.0f }); // 5: Base center
}

void Pyramid::GenerateVertices()
{
    float halfSize = baseSize * 0.5f;

    // Front face (z positive)
    glm::vec3 v0 = { -halfSize, 0.0f, halfSize };
    glm::vec3 v1 = { halfSize, 0.0f, halfSize };
    glm::vec3 v2 = { 0.0f, height, 0.0f };
    glm::vec3 frontNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    vertices.push_back({ v0, frontNormal, {0.0f, 0.0f} });
    vertices.push_back({ v1, frontNormal, {1.0f, 0.0f} });
    vertices.push_back({ v2, frontNormal, {0.5f, 1.0f} });

    // Right face (x positive)
    v0 = { halfSize, 0.0f,  halfSize };
    v1 = { halfSize, 0.0f, -halfSize };
    v2 = { 0.0f, height, 0.0f };
    glm::vec3 rightNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    vertices.push_back({ v0, rightNormal, {0.0f, 0.0f} });
    vertices.push_back({ v1, rightNormal, {1.0f, 0.0f} });
    vertices.push_back({ v2, rightNormal, {0.5f, 1.0f} });

    // Back face (z negative)
    v0 = { halfSize, 0.0f, -halfSize };
    v1 = { -halfSize, 0.0f, -halfSize };
    v2 = { 0.0f, height, 0.0f };
    glm::vec3 backNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    vertices.push_back({ v0, backNormal, {0.0f, 0.0f} });
    vertices.push_back({ v1, backNormal, {1.0f, 0.0f} });
    vertices.push_back({ v2, backNormal, {0.5f, 1.0f} });

    // Left face (x negative)
    v0 = { -halfSize, 0.0f, -halfSize };
    v1 = { -halfSize, 0.0f,  halfSize };
    v2 = { 0.0f, height, 0.0f };
    glm::vec3 leftNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    vertices.push_back({ v0, leftNormal, {0.0f, 0.0f} });
    vertices.push_back({ v1, leftNormal, {1.0f, 0.0f} });
    vertices.push_back({ v2, leftNormal, {0.5f, 1.0f} });

    // Base face (y negative)
    glm::vec3 baseNormal = { 0.0f, -1.0f, 0.0f };
    vertices.push_back({ {-halfSize, 0.0f, -halfSize}, baseNormal, {0.0f, 0.0f} });
    vertices.push_back({ { halfSize, 0.0f, -halfSize}, baseNormal, {1.0f, 0.0f} });
    vertices.push_back({ { halfSize, 0.0f,  halfSize}, baseNormal, {1.0f, 1.0f} });
    vertices.push_back({ {-halfSize, 0.0f,  halfSize}, baseNormal, {0.0f, 1.0f} });
}

void Pyramid::GenerateIndices()
{
    indices = {
        // Front face
        0, 1, 2,
        // Right face
        3, 4, 5,
        // Back face
        6, 7, 8,
        // Left face
        9, 10, 11,
        // Base face (two triangles)
        12, 13, 14,
        14, 15, 12
    };
}