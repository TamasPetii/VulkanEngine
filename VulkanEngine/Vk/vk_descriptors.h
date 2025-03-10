#pragma once
#include "vk_types.h"

struct DescriptorLayoutBuilder
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	void Clear();
	void AddBinding(uint32_t binding, VkDescriptorType type);
	VkDescriptorSetLayout Build(VkDevice logicalDevice, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};

struct DescriptorAllocator
{
	VkDescriptorPool pool;

	struct PoolSizeRatio {
		VkDescriptorType type;
		float ratio;
	};

	void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
	void DestroyPool(VkDevice device);
	void ClearDescriptors(VkDevice device);
	VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
};