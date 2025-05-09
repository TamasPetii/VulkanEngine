#pragma once
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/CommandBuffer.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"

#include <array>

class ENGINE_API Renderer
{
public:
	Renderer();
	~Renderer();
	void Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
	void SetGuiRenderFunction(const std::function<void(VkCommandBuffer, std::shared_ptr<Registry>, std::shared_ptr<ResourceManager>, uint32_t)>& function);
	void RecreateSwapChain();
private:
	void Init();
	void Destroy();
	void InitCommandPool();
	void InitCommandBuffer();
private:
	std::array<VkCommandPool, GlobalConfig::FrameConfig::maxFramesInFlights> commandPools;
	std::array<VkCommandBuffer, GlobalConfig::FrameConfig::maxFramesInFlights> commandBuffers;
	std::function<void(VkCommandBuffer, std::shared_ptr<Registry>, std::shared_ptr<ResourceManager>, uint32_t)> guiRenderFunction;
};

