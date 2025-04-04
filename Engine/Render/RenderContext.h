#pragma once
#include "../EngineApi.h"
#include "../Vulkan/Image.h"
#include "../Vulkan/ShaderModule.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/FrameBuffer.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/DynamicRendering.h"
#include "../Vulkan/ImageSampler.h"
#include "../Vulkan/DescriptorSet.h"
#include "../Vulkan/DescriptorPool.h"
#include "Engine/Config.h"

class Renderer;

class ENGINE_API RenderContext
{
public:
	~RenderContext();
	static RenderContext* GetContext();
	static void DestroyContext();
	uint32_t GetFramesInFlightIndex();
	std::shared_ptr<Vk::RenderPass> GetRenderPass(const std::string& name);
	std::shared_ptr<Vk::ShaderModule> GetShaderModule(const std::string& name);
	std::shared_ptr<Vk::GraphicsPipeline> GetGraphicsPipeline(const std::string& name);
	std::shared_ptr<Vk::ImageSampler> GetSampler(const std::string& name);
	std::shared_ptr<Vk::FrameBuffer> GetFrameBuffer(const std::string& name, uint32_t index);

	void MarkFrameBufferToResize(const std::string& name, uint32_t index, uint32_t width, uint32_t height);
	void ResizeMarkedFrameBuffers(uint32_t framesInFlightIndex);

	const auto& GetFrameBuffersToResize() { return frameBuffersToResize; }

	std::shared_ptr<Vk::DescriptorPool> descriptorPool;
	std::vector<std::shared_ptr<Vk::DescriptorSet>> descriptorSets;
private:
	RenderContext() = default;
	void Init();
	void Destory();
	static RenderContext* context;
	static bool initialized;
private:
	void InitShaderModules();
	void InitFrameBuffers();
	void InitDescriptors();
	void InitRenderPasses();
	void InitSamplers();
	void UpdateFramesInFlightIndex();
	void InitGraphicsPipelines();
private:
	bool viewPortResize;
	uint32_t framesInFlightIndex = 0;
	std::unordered_map<std::string, std::shared_ptr<Vk::ImageSampler>> samplers;
	std::unordered_map<std::string, std::shared_ptr<Vk::RenderPass>> renderPasses;
	std::unordered_map<std::string, std::shared_ptr<Vk::ShaderModule>> shaderModuls;
	std::unordered_map<std::string, std::shared_ptr<Vk::GraphicsPipeline>> graphicsPipelines;
	std::unordered_map<std::string, std::vector<std::shared_ptr<Vk::FrameBuffer>>> frameBuffers;
	std::set<std::tuple<std::string, uint32_t, uint32_t, uint32_t>> frameBuffersToResize;

	friend class Renderer;
};