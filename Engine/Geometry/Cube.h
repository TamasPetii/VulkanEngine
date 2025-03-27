#pragma once
#include "Shape.h"

class Cube : public Shape
{
public:
    Cube(float halfSize = 1.0f);
private:
    void GenerateSurfacePoints() override;
    void GenerateVertices() override;
    void GenerateIndices() override;
private:
    float halfSize;
};
