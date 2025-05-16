#pragma once
#include "BaseComponents/Component.h"
#include "Engine/EngineApi.h"
#include <stdint.h>

struct ENGINE_API RenderIndicesComponent : public Component
{
	uint32_t entityIndex;
	uint32_t transformIndex;
	uint32_t modelIndex;
	uint32_t animationIndex;
	uint32_t bitflag;
};

struct ENGINE_API RenderIndicesGPU
{
	uint32_t entityIndex;
	uint32_t transformIndex;
	uint32_t modelIndex; //Defines: VertexBuffer, MaterialBuffer, NodeTransformBuffer
	uint32_t animationIndex; //Defines: BoneBuffer
	uint32_t animationTransformIndex; //Defines: Unique NodeTransformBuffer
	uint32_t bitflag;
};