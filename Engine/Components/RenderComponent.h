#pragma once
#include "Component.h"

struct ENGINE_API RenderComponent : public Component
{
	bool toRender = true;
	bool castShadow;
	bool receiveShadow;
};

struct ENGINE_API RenderIndicesGPU
{
	uint32_t entityIndex;
	uint32_t transformIndex;
	uint32_t materialIndex;
	uint32_t receiveShadow;
	uint32_t animationIndex;
	uint32_t animationTransformIndex;
	uint32_t filler1;
	uint32_t filler2;
};