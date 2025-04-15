#pragma once
#include "Component.h"
#include "ComponentGPU.h"
#include "Engine/Vulkan/Image.h"
#include <glm/glm.hpp>

struct ENGINE_API MaterialComponent : public Component
{
	MaterialComponent();

	bool useBloom;
	glm::vec4 color;
	glm::vec2 uvScale;
	std::shared_ptr<Vk::Image> albedo;
	std::shared_ptr<Vk::Image> normal;
	std::shared_ptr<Vk::Image> metallic; 
	std::shared_ptr<Vk::Image> roughness;
};

struct ENGINE_API MaterialComponentGPU
{
	MaterialComponentGPU(const MaterialComponent& component);

	glm::vec4 color;
	glm::vec2 uvScale;
	glm::vec2 bloom;
	uint32_t albedoIndex;
	uint32_t normalIndex;
	uint32_t metallicIndex;
	uint32_t roughnessIndex;
};