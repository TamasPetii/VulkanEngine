#pragma once
#include "Shape.h"

class Quad : public Shape
{
public:
	Quad();
private:
	virtual void PopulateSurfacePoints() override;
	virtual void GenerateVertices() override;
	virtual void GenerateIndices() override;
};

