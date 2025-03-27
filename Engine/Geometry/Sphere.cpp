#include "Sphere.h"

Sphere::Sphere(uint32_t segments) : 
    radius(1.f),
    segments(segments)
{
    Initialize();
}

void Sphere::GenerateSurfacePoints()
{
    for (uint32_t i = 0; i <= segments; ++i)
    {
        float phi = glm::pi<float>() * static_cast<float>(i) / segments;  // 0 to π
       
        for (uint32_t j = 0; j <= segments; ++j)
        {
            float theta = 2.0f * glm::pi<float>() * static_cast<float>(j) / segments;  // 0 to 2π

            glm::vec3 point;
            point.x = radius * sinf(phi) * cosf(theta);
            point.y = radius * cosf(phi);
            point.z = radius * sinf(phi) * sinf(theta);

            surfacePoints.push_back(point);
        }
    }
}

void Sphere::GenerateVertices()
{
    for (uint32_t i = 0; i <= segments; ++i)
    {
        for (uint32_t j = 0; j <= segments; ++j)
        {
            Vertex vertex;
            vertex.position = surfacePoints[i * (segments + 1) + j];
            vertex.normal = glm::normalize(vertex.position);
            vertex.uv_x = static_cast<float>(j) / segments;
            vertex.uv_y = static_cast<float>(i) / segments;
            vertices.push_back(vertex);
        }
    }
}

void Sphere::GenerateIndices()
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