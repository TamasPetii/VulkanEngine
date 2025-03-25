#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"
#include <unordered_map>
#include <span>

namespace Vk
{
	struct ENGINE_API DescriptorLayoutConfig
	{
		uint32_t binding;
		VkDescriptorType type;
		VkShaderStageFlags flags;
	};

	struct ENGINE_API DescriptorLayoutBuffer
	{
		VkBuffer buffer;
		VkDeviceSize offset;
		VkDeviceSize range;
		DescriptorLayoutConfig config;
	};

	struct ENGINE_API DescriptorLayoutImage
	{
		VkImageView image;
		VkSampler sampler;
		VkImageLayout layout;
		DescriptorLayoutConfig config;
	};

	class ENGINE_API DescriptorSet
	{
	public:
		static VkDescriptorSetLayoutBinding BuildLayoutBindingInfo(uint32_t binding, VkDescriptorType type, VkShaderStageFlags flags);

		DescriptorSet(VkDescriptorPool pool, std::span<DescriptorLayoutBuffer> bufferLayouts, std::span<DescriptorLayoutImage> imageLayouts);
		~DescriptorSet();
		void Free(VkDescriptorPool pool);
		const VkDescriptorSet& Value() const;
		const VkDescriptorSetLayout& Layout() const;
	private:
		void Initialize(VkDescriptorPool pool, std::span<DescriptorLayoutBuffer> bufferLayouts, std::span<DescriptorLayoutImage> imageLayouts);
		void Cleanup();
	private:
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptorlayout = VK_NULL_HANDLE;
	};

	class ENGINE_API DescriptorSetBuilder
	{
	public:
		DescriptorSetBuilder& AddDescriptorLayoutBuffer(uint32_t binding, VkDescriptorType type, VkShaderStageFlags flags, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
		DescriptorSetBuilder& AddDescriptorLayoutImage(uint32_t binding, VkDescriptorType type, VkShaderStageFlags flags, VkImageView image, VkSampler sampler, VkImageLayout layout);
		std::shared_ptr<DescriptorSet> BuildDescriptorSet(VkDescriptorPool pool);
	private:
		std::vector<DescriptorLayoutBuffer> bufferLayouts;
		std::vector<DescriptorLayoutImage> imageLayouts;
	};
}