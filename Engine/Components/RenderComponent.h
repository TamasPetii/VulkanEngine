#pragma once
#include "BaseComponents/Component.h"
#include "BaseComponents/FrustumCullable.h"

struct ENGINE_API RenderComponent : public Component, public FrustumCullable
{
	bool castShadow;
	bool receiveShadow;
};