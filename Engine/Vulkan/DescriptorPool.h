#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"
#include <unordered_map>
#include <span>

namespace Vk
{
	class ENGINE_API DescriptorPool
	{
	public:
		DescriptorPool(std::span<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
		~DescriptorPool();
		const VkDescriptorPool& Value() const;
	private:
		void Initialize(std::span<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
		void Cleanup();
	private:
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	};

	class ENGINE_API DescriptorPoolBuilder
	{
	public:
		DescriptorPoolBuilder() = default;
		~DescriptorPoolBuilder();
		DescriptorPoolBuilder& Reset();
		DescriptorPoolBuilder& SetMaxSets(uint32_t size);
		DescriptorPoolBuilder& SetPoolSize(VkDescriptorType type, uint32_t size);
		std::shared_ptr<DescriptorPool> BuildDescriptorPool();
	private:
		uint32_t maxSets = 0;
		std::unordered_map<VkDescriptorType, uint32_t> poolSizes;
	};
}


