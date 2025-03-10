#include "vk_descriptors.h"

void DescriptorLayoutBuilder::Clear()
{
	bindings.clear();
}

void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type)
{
	VkDescriptorSetLayoutBinding bindingInfo{};
	bindingInfo.binding = binding;
	bindingInfo.descriptorCount = 1;
	bindingInfo.descriptorType = type;

	bindings.push_back(bindingInfo);
}

VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice logicalDevice, VkShaderStageFlags shaderStages, void* pNext, VkDescriptorSetLayoutCreateFlags flags)
{
	for (auto& bindingInfo : bindings)
		bindingInfo.stageFlags |= shaderStages;

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = pNext;
	layoutCreateInfo.flags = flags;

	layoutCreateInfo.bindingCount = (uint32_t)bindings.size();
	layoutCreateInfo.pBindings = bindings.data();

	VkDescriptorSetLayout setLayout;
	VK_CHECK(vkCreateDescriptorSetLayout(logicalDevice, &layoutCreateInfo, nullptr, &setLayout), "Couldn't create descriptor set layout!");

	return setLayout;
}

void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
{
	std::vector<VkDescriptorPoolSize> poolSizes;

	for (PoolSizeRatio ratio : poolRatios) {
		VkDescriptorPoolSize poolSize;
		poolSize.descriptorCount = uint32_t(ratio.ratio * maxSets);
		poolSize.type = ratio.type;
		poolSizes.push_back(poolSize);
	}

	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = maxSets;
	poolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
	poolCreateInfo.pPoolSizes = poolSizes.data();
	poolCreateInfo.flags = 0;

	VK_CHECK(vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &pool), "Couldn't create descriptor pool");
}

void DescriptorAllocator::DestroyPool(VkDevice device)
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}

void DescriptorAllocator::ClearDescriptors(VkDevice device)
{
	VK_CHECK(vkResetDescriptorPool(device, pool, 0), "Couldn't reset descriptor pool");
}

VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout;

	VkDescriptorSet set;
	VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &set), "Couldn't allocate descriptor set");

	return set;
}
