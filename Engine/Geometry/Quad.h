#pragma once
#include "Shape.h"

class Quad : public Shape
{
public:
	Quad();
private:
	void GenerateSurfacePoints() override;
	void GenerateVertices() override;
	void GenerateIndices() override;
};

