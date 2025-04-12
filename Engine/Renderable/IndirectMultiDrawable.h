#pragma once
#include <array>
#include <algorithm>
#include <glm/glm.hpp>
#include "Engine/Config.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"

class ENGINE_API IndirectMultiDrawable
{
public:
	std::shared_ptr<Vk::Buffer> GetIndirectBuffer(uint32_t frameIndex);
	void UpdateIndirectCommandsInstanceCount(uint32_t instanceCount);
	virtual void UploadIndirectCommandsToGpu(uint32_t frameIndex);
protected:
	std::vector<VkDrawIndexedIndirectCommand> indirectCommands;
	std::array<std::shared_ptr<Vk::Buffer>, Settings::MAX_FRAMES_IN_FLIGHTS> indirectBuffer;
};

