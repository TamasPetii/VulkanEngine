#include "Cube.h"

Cube::Cube(float halfSize) : halfSize(halfSize)
{
    Initialize();
}

void Cube::PopulateSurfacePoints()
{
    surfacePoints = {
        // Front face (z = +halfSize)
        {-halfSize, -halfSize, halfSize},  // 0
        { halfSize, -halfSize, halfSize},  // 1
        { halfSize,  halfSize, halfSize},  // 2
        {-halfSize,  halfSize, halfSize},  // 3

        // Back face (z = -halfSize)
        {-halfSize, -halfSize, -halfSize}, // 4
        { halfSize, -halfSize, -halfSize}, // 5
        { halfSize,  halfSize, -halfSize}, // 6
        {-halfSize,  halfSize, -halfSize}  // 7
    };
}

void Cube::GenerateVertices()
{
    // Front face (+Z)
    vertices.push_back({ {-halfSize, -halfSize, halfSize}, {0, 0, 1}, {0, 0} });
    vertices.push_back({ { halfSize, -halfSize, halfSize}, {0, 0, 1}, {1, 0} });
    vertices.push_back({ { halfSize,  halfSize, halfSize}, {0, 0, 1}, {1, 1} });
    vertices.push_back({ {-halfSize,  halfSize, halfSize}, {0, 0, 1}, {0, 1} });

    // Back face (-Z)
    vertices.push_back({ {-halfSize,  halfSize, -halfSize}, {0, 0, -1}, {0, 1} });
    vertices.push_back({ { halfSize,  halfSize, -halfSize}, {0, 0, -1}, {1, 1} });
    vertices.push_back({ { halfSize, -halfSize, -halfSize}, {0, 0, -1}, {1, 0} });
    vertices.push_back({ {-halfSize, -halfSize, -halfSize}, {0, 0, -1}, {0, 0} });

    // Right face (+X)
    vertices.push_back({ { halfSize, -halfSize,  halfSize}, {1, 0, 0}, {0, 0} });
    vertices.push_back({ { halfSize, -halfSize, -halfSize}, {1, 0, 0}, {1, 0} });
    vertices.push_back({ { halfSize,  halfSize, -halfSize}, {1, 0, 0}, {1, 1} });
    vertices.push_back({ { halfSize,  halfSize,  halfSize}, {1, 0, 0}, {0, 1} });

    // Left face (-X)
    vertices.push_back({ {-halfSize,  halfSize,  halfSize}, {-1, 0, 0}, {1, 1} });
    vertices.push_back({ {-halfSize,  halfSize, -halfSize}, {-1, 0, 0}, {0, 1} });
    vertices.push_back({ {-halfSize, -halfSize, -halfSize}, {-1, 0, 0}, {0, 0} });
    vertices.push_back({ {-halfSize, -halfSize,  halfSize}, {-1, 0, 0}, {1, 0} });

    // Top face (+Y)
    vertices.push_back({ {-halfSize,  halfSize,  halfSize}, {0, 1, 0}, {0, 1} });
    vertices.push_back({ { halfSize,  halfSize,  halfSize}, {0, 1, 0}, {1, 1} });
    vertices.push_back({ { halfSize,  halfSize, -halfSize}, {0, 1, 0}, {1, 0} });
    vertices.push_back({ {-halfSize,  halfSize, -halfSize}, {0, 1, 0}, {0, 0} });

    // Bottom face (-Y)
    vertices.push_back({ {-halfSize, -halfSize, -halfSize}, {0, -1, 0}, {0, 0} });
    vertices.push_back({ { halfSize, -halfSize, -halfSize}, {0, -1, 0}, {1, 0} });
    vertices.push_back({ { halfSize, -halfSize,  halfSize}, {0, -1, 0}, {1, 1} });
    vertices.push_back({ {-halfSize, -halfSize,  halfSize}, {0, -1, 0}, {0, 1} });
}

void Cube::GenerateIndices()
{
    indices = {
        // Front
        0, 1, 2,   2, 3, 0,
        // Back
        4, 5, 6,   6, 7, 4,
        // Right
        8, 9, 10,  10, 11, 8,
        // Left
        12, 13, 14, 14, 15, 12,
        // Top
        16, 17, 18, 18, 19, 16,
        // Bottom
        20, 21, 22, 22, 23, 20
    };
}