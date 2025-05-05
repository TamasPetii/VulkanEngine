#pragma once
#include "Component.h"
#include "Engine/Config.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Animation/Animation.h"
#include "Engine/Renderable/Model/NodeTransform.h"
#include <memory>
#include <array>

struct ENGINE_API AnimationNodeTransform
{
	glm::mat4 localTransform = glm::mat4(1); //Parent node without offset
	NodeTransform nodeTransform; // Contains vertex local offset matrix
};

struct ENGINE_API AnimationComponent : public Component
{
	struct AnimationTransformBuffer
	{
		uint32_t version = 0;
		std::shared_ptr<Vk::Buffer> buffer;
	};

	AnimationComponent();

	double time;
	float speed;
	std::shared_ptr<Animation> animation;
	std::vector<AnimationNodeTransform> animationNodeTransforms;
	std::array<AnimationTransformBuffer, GlobalConfig::FrameConfig::maxFramesInFlights> nodeTransformBuffers;
	uint32_t nodeTransformVersion = 0; //Systems need this to know which buffer is outdated.
};