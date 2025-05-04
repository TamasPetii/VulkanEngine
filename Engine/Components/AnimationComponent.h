#pragma once
#include "Component.h"
#include <memory>
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Animation/Animation.h"
#include "Engine/Renderable/Model/NodeTransform.h"

struct ENGINE_API AnimationComponent : public Component
{
	AnimationComponent();

	double time;
	float speed;
	std::vector<NodeTransform> transforms;
	std::shared_ptr<Animation> animation;
	std::shared_ptr<Vk::Buffer> nodeTransformBuffer;
};