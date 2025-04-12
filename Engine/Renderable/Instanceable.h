#pragma once
#include <array>
#include <glm/glm.hpp>
#include "Engine/Config.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"


class ENGINE_API Instanceable
{
public:
	struct Indices
	{
		uint32_t entityIndex;
		uint32_t transformIndex;
		uint32_t materialIndex;
		uint32_t fillerIndex;
	};
public:
	uint32_t GetInstanceCount();
	void AddIndex(const Indices& index);
	virtual void UploadInstanceDataToGPU(uint32_t frameIndex);
	std::shared_ptr<Vk::Buffer> GetInstanceIndexBuffer(uint32_t frameIndex);
protected:
	uint32_t instanceCount = 0;
	std::vector<Indices> instanceIndices;
	std::array<std::shared_ptr<Vk::Buffer>, MAX_FRAMES_IN_FLIGHTS> instanceIndexBuffers;
};

