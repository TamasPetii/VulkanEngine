#pragma once
#include "../Vulkan/VulkanContext.h"
#include "../Vulkan/ShaderModule.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/Image.h"
#include "../Vulkan/FrameBuffer.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/DynamicRendering.h"
#include "Vertex.h"

constexpr uint32_t FRAMES_IN_FLIGHT = 2;

class Renderer
{
public:
	Renderer();
	~Renderer();
	void Render();
private:
	void Init();
	void Destroy();
	void LoadShaders();
	void InitCommandPool();
	void InitCommandBuffer();
	void InitSyncronization();
	void InitRenderPass();
	void InitFrameBuffers();
	void InitGraphicsPipeline();
private:
	std::vector<VkCommandPool> commandPools;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	std::shared_ptr<Vk::RenderPass> renderPass;
	std::shared_ptr<Vk::GraphicsPipeline> graphicsPipeline;
	std::vector<std::shared_ptr<Vk::FrameBuffer>> frameBuffers;
	std::unordered_map<std::string, std::shared_ptr<Vk::ShaderModule>> shaderModuls;

	uint32_t framesInFlightIndex = 0;
};

