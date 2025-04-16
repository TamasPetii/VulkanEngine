#pragma once
#include <array>
#include <glm/glm.hpp>
#include "Engine/Config.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"


class ENGINE_API Instanceable
{
public:
	uint32_t GetInstanceCount();
	void ResetInstanceCount();
	void ReserveInstances(uint32_t size);
	void ShrinkInstances();
	void AddIndex(uint32_t index);
	virtual void UploadInstanceDataToGPU(uint32_t frameIndex);
	std::shared_ptr<Vk::Buffer> GetInstanceIndexBuffer(uint32_t frameIndex);
protected:
	uint32_t instanceCount = 0;
	std::vector<uint32_t> instanceIndices;
	std::array<std::shared_ptr<Vk::Buffer>, Settings::MAX_FRAMES_IN_FLIGHTS> instanceIndexBuffers;
};

