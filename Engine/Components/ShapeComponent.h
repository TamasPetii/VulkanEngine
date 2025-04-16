#pragma once
#include "Component.h"
#include "Engine/Renderable/Geometry/Shape.h"

struct ENGINE_API ShapeComponent : public Component
{
	bool toRender;
	bool castShadow;
	bool receiveShadow;
	std::shared_ptr<Shape> shape;
};

struct ENGINE_API ShapeComponentGPU
{
	uint32_t entityIndex;
	uint32_t transformIndex;
	uint32_t materialIndex;
	uint32_t receiveShadow;
};