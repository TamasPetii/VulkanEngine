#pragma once
#include <glm/gtc/constants.hpp>
#include "Engine/Renderable/Renderable.h"

class ENGINE_API Shape : public Renderable
{
public:

protected:
	void Initialize();
	virtual void GenerateSurfacePoints() = 0;
	virtual void GenerateVertices() = 0;
	virtual void GenerateIndices() = 0;
	void GenerateTangents();
};

