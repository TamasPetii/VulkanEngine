#pragma once
#include "../Vulkan/VulkanContext.h"
#include "../Vulkan/Buffer.h"
#include "../Vulkan/CommandBuffer.h"
#include "../Vulkan/Fence.h"
#include "../Vulkan/Semaphore.h"
#include "RenderContext.h"
#include "Vertex.h"

#include "Renderers/DeferredRenderer.h"
#include "Renderers/GeometryRenderer.h"
#include "Engine/Geometry/Geometries.h"

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

	std::vector<VkCommandPool> commandPools;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<std::shared_ptr<Vk::Fence>> inFlightFences;
	std::vector<std::shared_ptr<Vk::Semaphore>> imageAvailableSemaphores;
	std::vector<std::shared_ptr<Vk::Semaphore>> renderFinishedSemaphores;


	std::shared_ptr<Vk::Buffer> indexBuffer;
	std::shared_ptr<Vk::Buffer> vertexBuffer;

	std::unordered_map<std::string, std::shared_ptr<Shape>> shapes;

	std::function<void(VkCommandBuffer commandBuffer)> guiRenderFunction;
};

