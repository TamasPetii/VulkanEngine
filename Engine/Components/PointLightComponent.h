#pragma once
#include "BaseComponents/VersionIndexed.h"
#include "BaseComponents/Component.h"
#include "BaseComponents/FrustumCullable.h"

#include "Engine/Vulkan/FrameBuffer.h"
#include "Engine/Config.h"
#include <glm/glm.hpp>
#include <memory>
#include <array>

struct ENGINE_API PointLightShadowFrameBuffer : public VersionIndexed
{
	std::shared_ptr<Vk::FrameBuffer> frameBuffer = nullptr;
};

struct ENGINE_API PointLightShadow : public VersionIndexed
{
	PointLightShadow();

	bool use;
	uint32_t textureSize;
	uint32_t updateFrequency;
	std::array<PointLightShadowFrameBuffer, GlobalConfig::FrameConfig::maxFramesInFlights> frameBuffers;
	//Todo: viewProjs
};

struct ENGINE_API PointLightComponent : public Component, public FrustumCullable
{
	PointLightComponent();

	glm::vec3 color;
	glm::vec3 position; //Mapped to transform component translation!
	float radius; //Mapped to transform component scale (max value)
	float weakenDistance;
	float strength;
	float shininess;
	PointLightShadow shadow;
	//Todo: Visible entities??? Instanced???? Maybe in models???? Or like a model map?
};

struct ENGINE_API PointLightGPU
{
	PointLightGPU(const PointLightComponent& pointLightComponent);

	glm::vec3 color;
	float strength;
	glm::vec3 position;
	float shininess;
	float radius;
	float weakenDistance;
	uint32_t bitflag;
	uint32_t padding;
	//Todo: shadow viewProjs
};