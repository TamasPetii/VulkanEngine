#pragma once
#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(uint32_t segments = 32);
private:
	void GenerateSurfacePoints() override;
	void GenerateVertices() override;
	void GenerateIndices() override;
private:
	float radius;
	uint32_t segments;
};

