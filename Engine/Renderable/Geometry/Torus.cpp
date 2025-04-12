#include "Torus.h"

Torus::Torus(float majorRadius, float minorRadius, uint32_t majorSegments, uint32_t minorSegments)
    : majorRadius(majorRadius), minorRadius(minorRadius), majorSegments(majorSegments), minorSegments(minorSegments)
{
    Initialize();
}

void Torus::PopulateSurfacePoints()
{
    for (uint32_t i = 0; i <= majorSegments; ++i)
    {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / majorSegments;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (uint32_t j = 0; j <= minorSegments; ++j)
        {
            float phi = 2.0f * glm::pi<float>() * static_cast<float>(j) / minorSegments;
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            glm::vec3 point;
            point.x = (majorRadius + minorRadius * cosPhi) * cosTheta;
            point.y = minorRadius * sinPhi;
            point.z = (majorRadius + minorRadius * cosPhi) * sinTheta;

            surfacePoints.push_back(point);
        }
    }
}

void Torus::GenerateVertices()
{
    for (uint32_t i = 0; i <= majorSegments; ++i)
    {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / majorSegments;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (uint32_t j = 0; j <= minorSegments; ++j)
        {
            float phi = 2.0f * glm::pi<float>() * static_cast<float>(j) / minorSegments;
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            Vertex vertex;
            vertex.position = surfacePoints[i * (minorSegments + 1) + j];

            // Calculate normal
            glm::vec3 centerOfTube = glm::vec3(majorRadius * cosTheta, 0.0f, majorRadius * sinTheta);
            vertex.normal = glm::normalize(vertex.position - centerOfTube);

            // UV coordinates
            vertex.uv_x = static_cast<float>(i) / majorSegments;
            vertex.uv_y = static_cast<float>(j) / minorSegments;

            vertices.push_back(vertex);
        }
    }
}

void Torus::GenerateIndices()
{
    for (uint32_t i = 0; i < majorSegments; ++i)
    {
        for (uint32_t j = 0; j < minorSegments; ++j)
        {
            uint32_t topLeft = i * (minorSegments + 1) + j;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (i + 1) * (minorSegments + 1) + j;
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