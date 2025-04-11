#pragma once
#include "Shape.h"

class Pyramid : public Shape
{
public:
    Pyramid(float baseSize = 2.0f, float height = 2.0f);
private:
    void GenerateSurfacePoints() override;
    void GenerateVertices() override;
    void GenerateIndices() override;
private:
    float baseSize;
    float height;
};
