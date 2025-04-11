#include "Cylinder.h"

Cylinder::Cylinder(float radius, float height, uint32_t segments)
    : radius(radius), height(height), segments(segments)
{
    Initialize();
}

void Cylinder::GenerateSurfacePoints()
{
    float halfHeight = height * 0.5f;

    // Side points (two rings)
    for (uint32_t i = 0; i < 2; ++i)
    {
        float y = halfHeight - static_cast<float>(i) * height;
        for (uint32_t j = 0; j <= segments; ++j)
        {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / segments;
            glm::vec3 point;
            point.x = radius * cosf(theta);
            point.y = y;
            point.z = radius * sinf(theta);
            surfacePoints.push_back(point);
        }
    }

    // Center points for top and bottom caps
    surfacePoints.push_back({ 0.0f, halfHeight, 0.0f });  // Top center
    surfacePoints.push_back({ 0.0f, -halfHeight, 0.0f }); // Bottom center
}

void Cylinder::GenerateVertices()
{
    // Side vertices
    for (uint32_t i = 0; i < 2; ++i)
    {
        float v = static_cast<float>(i);
        for (uint32_t j = 0; j <= segments; ++j)
        {
            Vertex vertex;
            vertex.position = surfacePoints[i * (segments + 1) + j];
            vertex.normal = glm::normalize(glm::vec3(vertex.position.x, 0.0f, vertex.position.z));

            // Linear UV for sides
            vertex.uv_x = static_cast<float>(j) / segments;
            vertex.uv_y = v;

            vertices.push_back(vertex);
        }
    }

    // Top cap vertices
    uint32_t topCenterIdx = 2 * (segments + 1);
    for (uint32_t j = 0; j <= segments; ++j)
    {
        Vertex vertex;
        vertex.position = surfacePoints[j];  // Top ring
        vertex.normal = { 0.0f, 1.0f, 0.0f };

        // Circular UV coordinates
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / segments;
        vertex.uv_x = 0.5f + 0.5f * cosf(theta);  // Center at (0.5, 0.5)
        vertex.uv_y = 0.5f + 0.5f * sinf(theta);

        vertices.push_back(vertex);
    }
    vertices.push_back({ surfacePoints[topCenterIdx], {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f} }); // Top center

    // Bottom cap vertices
    uint32_t bottomCenterIdx = topCenterIdx + 1;
    for (uint32_t j = 0; j <= segments; ++j)
    {
        Vertex vertex;
        vertex.position = surfacePoints[(segments + 1) + j];  // Bottom ring
        vertex.normal = { 0.0f, -1.0f, 0.0f };

        // Circular UV coordinates
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / segments;
        vertex.uv_x = 0.5f + 0.5f * cosf(theta);
        vertex.uv_y = 0.5f + 0.5f * sinf(theta);

        vertices.push_back(vertex);
    }
    vertices.push_back({ surfacePoints[bottomCenterIdx], {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f} }); // Bottom center
}

void Cylinder::GenerateIndices()
{
    for (uint32_t j = 0; j < segments; ++j)
    {
        uint32_t topLeft = j;
        uint32_t topRight = j + 1;
        uint32_t bottomLeft = j + (segments + 1);
        uint32_t bottomRight = j + (segments + 1) + 1;

        indices.push_back(topLeft);
        indices.push_back(topRight);
        indices.push_back(bottomLeft);

        indices.push_back(topRight);
        indices.push_back(bottomRight);
        indices.push_back(bottomLeft);
    }

    // Top cap
    uint32_t topStart = 2 * (segments + 1);
    uint32_t topCenter = topStart + (segments + 1);
    for (uint32_t j = 0; j < segments; ++j)
    {
        indices.push_back(topStart + j);
        indices.push_back(topCenter);
        indices.push_back(topStart + j + 1);
    }

    // Bottom cap
    uint32_t bottomStart = topCenter + 1;
    uint32_t bottomCenter = bottomStart + (segments + 1);
    for (uint32_t j = 0; j < segments; ++j)
    {
        indices.push_back(bottomStart + j + 1);
        indices.push_back(bottomCenter);
        indices.push_back(bottomStart + j);
    }
}