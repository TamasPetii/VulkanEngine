#pragma once
#include "../EngineApi.h"
#include "../Vulkan/Image.h"
#include "../Vulkan/ShaderModule.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/FrameBuffer.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/DynamicRendering.h"

constexpr uint32_t FRAMES_IN_FLIGHT = 2;

class Renderer;

class ENGINE_API RenderContext
{
public:
	~RenderContext();
	static RenderContext* GetContext();
	static void DestroyContext();
	std::shared_ptr<Vk::RenderPass> GetRenderPass(const std::string& name);
	std::shared_ptr<Vk::ShaderModule> GetShaderModule(const std::string& name);
	std::shared_ptr<Vk::GraphicsPipeline> GetGraphicsPipeline(const std::string& name);
	std::shared_ptr<Vk::FrameBuffer> GetFrameBuffer(const std::string& name, uint32_t index);
	const VkSampler& GetSampler(const std::string& name);
private:
	RenderContext() = default;
	void Init();
	void Destory();
	static RenderContext* context;
	static bool initialized;
private:
	void InitShaderModules();
	void InitFrameBuffers();
	void InitRenderPasses();
	void InitGraphicsPipelines();
	void InitSamplers();
private:
	std::unordered_map<std::string, std::shared_ptr<Vk::RenderPass>> renderPasses;
	std::unordered_map<std::string, std::shared_ptr<Vk::ShaderModule>> shaderModuls;
	std::unordered_map<std::string, std::shared_ptr<Vk::GraphicsPipeline>> graphicsPipelines;
	std::unordered_map<std::string, std::vector<std::shared_ptr<Vk::FrameBuffer>>> frameBuffers;
	std::unordered_map<std::string, VkSampler> samplers;

	friend class Renderer;
};