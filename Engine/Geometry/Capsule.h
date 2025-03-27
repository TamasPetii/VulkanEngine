#pragma once
#include "Shape.h"

class Capsule : public Shape
{
public:
    Capsule(float radius = 1.0f, float height = 1.0f, uint32_t segments = 32);
private:
    void GenerateSurfacePoints() override;
    void GenerateVertices() override;
    void GenerateIndices() override;
private:
    float radius;
    float height;
    uint32_t segments;
};

