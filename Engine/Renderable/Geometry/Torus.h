#pragma once
#include "Shape.h"

class Torus : public Shape
{
public:
    Torus(float majorRadius = 0.75f, float minorRadius = 0.25f, uint32_t majorSegments = 32, uint32_t minorSegments = 32);
private:
    void GenerateSurfacePoints() override;
    void GenerateVertices() override;
    void GenerateIndices() override;
private:
    float majorRadius;    // Distance from center to tube center
    float minorRadius;    // Radius of the tube
    uint32_t majorSegments; // Number of segments around the main ring
    uint32_t minorSegments; // Number of segments around the tube
};