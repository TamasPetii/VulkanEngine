#pragma once
#include "BaseComponents/VersionIndexed.h"
#include "BaseComponents/Component.h"

#include "Engine/Vulkan/FrameBuffer.h"
#include "Engine/Config.h"
#include <glm/glm.hpp>
#include <memory>
#include <array>

constexpr glm::vec3 defaultDirectionLightDirection = glm::vec3(0.f, 0.f, 1.f);

struct ENGINE_API DirectionLightShadowFrameBuffer : public VersionIndexed
{
	std::shared_ptr<Vk::FrameBuffer> frameBuffer = nullptr;
};

struct ENGINE_API DirectionLightShadow : public VersionIndexed
{
	DirectionLightShadow();

	bool use;
	uint32_t textureSize;
	uint32_t updateFrequency;
	std::array<DirectionLightShadowFrameBuffer, GlobalConfig::FrameConfig::maxFramesInFlights> frameBuffers;
};

struct ENGINE_API DirectionLightComponent : public Component
{
	DirectionLightComponent();

	glm::vec3 color;
	glm::vec3 direction;
	float strength;
	float shininess;
	DirectionLightShadow shadow;
};

struct ENGINE_API DirectionLightGPU
{
	DirectionLightGPU(const DirectionLightComponent& directionLightComponent);

	glm::vec3 color;
	float strength;
	glm::vec3 direction;
	float shininess;
};

