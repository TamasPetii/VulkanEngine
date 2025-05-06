#pragma once
#include "Engine/EngineApi.h"
#include "Engine/Vulkan/Buffer.h"

class ENGINE_API BatchUploaded
{
public:
	struct ENGINE_API PendingUploadData
	{
		VkCommandPool commandPool = VK_NULL_HANDLE;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkCommandBufferSubmitInfo submitInfo{};
		std::shared_ptr<Vk::Buffer> stagingBuffer = nullptr;
	};
public:
	const VkCommandBufferSubmitInfo& GetCommandBufferSubmitInfo() const { return uploadData.submitInfo; }
	void CreateCommandPoolAndBuffer();
	void DestoryCommandPoolAndBuffer();
protected:
	void BeginCommandBuffer();
	void EndCommandBuffer();
	PendingUploadData uploadData;
};