#pragma once
#include "../Vulkan/VulkanContext.h"
#include "../Vulkan/ShaderModule.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/Image.h"
#include "../Vulkan/FrameBuffer.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "Vertex.h"

class Renderer
{
public:
	Renderer();
	void Render();
private:
	void Init();
	void InitCommandPool();
	void InitCommandBuffer();
	void InitSyncronization();
private:
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	std::shared_ptr<Vk::RenderPass> renderPass;
	std::shared_ptr<Vk::FrameBuffer> frameBuffer;
	std::shared_ptr<Vk::GraphicsPipeline> graphicsPipeline;
	std::unordered_map<std::string, std::shared_ptr<Vk::ShaderModule>> shaderModuls;
};

