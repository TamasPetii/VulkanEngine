#pragma once
#include "RenderComponent.h"
#include "Engine/Renderable/Model/Model.h"

struct ENGINE_API ModelComponent : public RenderComponent
{
	std::shared_ptr<Model> model;
};

