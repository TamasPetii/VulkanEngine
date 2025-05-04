#pragma once
#include "Component.h"
#include <memory>
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Animation/Animation.h"

struct ENGINE_API AnimationComponent : public Component
{
	float time;
	float speed;
	std::shared_ptr<Animation> animation;
	std::shared_ptr<Vk::Buffer> nodeTransformBuffer;
};