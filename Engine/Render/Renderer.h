#pragma once
#include "../Vulkan/VulkanContext.h"
#include "../Vulkan/Buffer.h"
#include "../Vulkan/CommandBuffer.h"
#include "RenderContext.h"
#include "Vertex.h"

class ENGINE_API Renderer
{
public:
	Renderer();
	~Renderer();
	void Render();
	void SetGuiRenderFunction(const std::function<void(VkCommandBuffer commandBuffer)>& function);
	void RecreateSwapChain();
private:
	void Init();
	void Destroy();
	void InitCommandPool();
	void InitCommandBuffer();
	void InitSyncronization();
	void InitBuffers();
private:
	VkCommandPool immediatePool;

	std::vector<VkFence> inFlightFences;
	std::vector<VkCommandPool> commandPools;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;

	std::unique_ptr<Vk::Buffer> indexBuffer;
	std::unique_ptr<Vk::Buffer> vertexBuffer;

	std::function<void(VkCommandBuffer commandBuffer)> guiRenderFunction;
};

