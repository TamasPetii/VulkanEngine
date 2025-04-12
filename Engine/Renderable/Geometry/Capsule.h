#pragma once
#include "Shape.h"

class Capsule : public Shape
{
public:
    Capsule(float radius = 1.0f, float height = 1.0f, uint32_t segments = 32);
private:
    virtual void PopulateSurfacePoints() override;
    virtual void GenerateVertices() override;
    virtual void GenerateIndices() override;
private:
    float radius;
    float height;
    uint32_t segments;
};

