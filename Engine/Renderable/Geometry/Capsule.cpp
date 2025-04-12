#include "Capsule.h"

Capsule::Capsule(float radius, float height, uint32_t segments)
    : radius(radius), height(height), segments(segments)
{
    Initialize();
}

void Capsule::PopulateSurfacePoints()
{
    // Top hemisphere (0 to π/2)
    for (uint32_t i = 0; i <= segments / 2; ++i)
    {
        float phi = glm::pi<float>() * 0.5f * static_cast<float>(i) / (segments / 2);
        for (uint32_t j = 0; j <= segments; ++j)
        {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / segments;

            glm::vec3 point;
            point.x = radius * sinf(phi) * cosf(theta);
            point.y = radius * cosf(phi) + height * 0.5f;  // Offset by half height
            point.z = radius * sinf(phi) * sinf(theta);

            surfacePoints.push_back(point);
        }
    }

    // Bottom hemisphere (π/2 to π)
    for (uint32_t i = 0; i <= segments / 2; ++i)
    {
        float phi = glm::pi<float>() * 0.5f + glm::pi<float>() * 0.5f * static_cast<float>(i) / (segments / 2);
        for (uint32_t j = 0; j <= segments; ++j)
        {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / segments;

            glm::vec3 point;
            point.x = radius * sinf(phi) * cosf(theta);
            point.y = radius * cosf(phi) - height * 0.5f;  // Offset by half height
            point.z = radius * sinf(phi) * sinf(theta);

            surfacePoints.push_back(point);
        }
    }
}

void Capsule::GenerateVertices()
{
    for (uint32_t i = 0; i <= segments; ++i)
    {
        for (uint32_t j = 0; j <= segments; ++j)
        {
            Vertex vertex;
            vertex.position = surfacePoints[i * (segments + 1) + j];

            // Calculate normal
            vertex.normal = glm::normalize(glm::vec3(
                vertex.position.x,
                vertex.position.y - (vertex.position.y > 0 ? height * 0.5f : -height * 0.5f),
                vertex.position.z
            ));

            // Calculate UV coordinates
            vertex.uv_x = static_cast<float>(j) / segments;
            vertex.uv_y = static_cast<float>(i) / segments;

            vertices.push_back(vertex);
        }
    }
}

void Capsule::GenerateIndices()
{
    for (uint32_t i = 0; i < segments; ++i)
    {
        for (uint32_t j = 0; j < segments; ++j)
        {
            uint32_t topLeft = i * (segments + 1) + j;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (i + 1) * (segments + 1) + j;
            uint32_t bottomRight = bottomLeft + 1;

            // First triangle
            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);

            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);
        }
    }
}