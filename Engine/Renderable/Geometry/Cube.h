#pragma once
#include "Shape.h"

class Cube : public Shape
{
public:
    Cube(float halfSize = 1.0f);
private:
    virtual void PopulateSurfacePoints() override;
    virtual void GenerateVertices() override;
    virtual void GenerateIndices() override;
private:
    float halfSize;
};
