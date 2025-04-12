#pragma once
#include "Shape.h"

class Pyramid : public Shape
{
public:
    Pyramid(float baseSize = 2.0f, float height = 2.0f);
private:
    virtual void PopulateSurfacePoints() override;
    virtual void GenerateVertices() override;
    virtual void GenerateIndices() override;
private:
    float baseSize;
    float height;
};
