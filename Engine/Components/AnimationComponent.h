#pragma once
#include "BaseComponents/VersionIndexed.h"
#include "BaseComponents/Component.h"
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

struct ENGINE_API AnimationNodeTransformBuffer : public VersionIndexed
{
	std::shared_ptr<Vk::Buffer> buffer;
};

struct ENGINE_API AnimationNodeTransformBuffers : public VersionIndexed
{
	std::array<AnimationNodeTransformBuffer, GlobalConfig::FrameConfig::maxFramesInFlights> buffers;
};

struct ENGINE_API AnimationComponent : public Component
{
	AnimationComponent();

	double time;
	float speed;
	std::shared_ptr<Animation> animation;
	std::vector<AnimationNodeTransform> nodeTransforms;
	AnimationNodeTransformBuffers nodeTransformBuffers;
};