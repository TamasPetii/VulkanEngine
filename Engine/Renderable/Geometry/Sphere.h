#pragma once
#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(uint32_t segments = 32);
private:
	virtual void PopulateSurfacePoints() override;
	virtual void GenerateVertices() override;
	virtual void GenerateIndices() override;
private:
	float radius;
	uint32_t segments;
};

