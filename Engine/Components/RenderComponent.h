#pragma once
#include "Component.h"

struct ENGINE_API RenderComponent : public Component
{
	bool toRender = true;
	bool castShadow;
	bool receiveShadow;
};