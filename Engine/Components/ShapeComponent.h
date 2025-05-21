#pragma once
#include "RenderComponent.h"
#include "BaseComponents/FrustumCullable.h"
#include "Engine/Renderable/Geometry/Shape.h"

struct ENGINE_API ShapeComponent : public RenderComponent
{
	std::shared_ptr<Shape> shape;
};