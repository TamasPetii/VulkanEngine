#pragma once
#include "../Vulkan/VulkanContext.h"
#include "RenderContext.h"
#include "Vertex.h"

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Render();
	void SetGuiRenderFunction(const std::function<void(VkCommandBuffer commandBuffer)>& function);
private:
	void Init();
	void Destroy();
	void InitCommandPool();
	void InitCommandBuffer();
	void InitSyncronization();
private:
	uint32_t framesInFlightIndex = 0;

	std::vector<VkFence> inFlightFences;
	std::vector<VkCommandPool> commandPools;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;

	std::function<void(VkCommandBuffer commandBuffer)> guiRenderFunction;
};

