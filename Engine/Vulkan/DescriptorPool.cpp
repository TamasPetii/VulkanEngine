#include "DescriptorPool.h"


Vk::DescriptorPool::DescriptorPool(std::span<VkDescriptorPoolSize> poolSizes, uint32_t maxSets)
{
	Initialize(poolSizes, maxSets);
}

Vk::DescriptorPool::~DescriptorPool()
{
	Cleanup();
}

const VkDescriptorPool& Vk::DescriptorPool::Value() const
{
	return descriptorPool;
}

void Vk::DescriptorPool::Initialize(std::span<VkDescriptorPoolSize> poolSizes, uint32_t maxSets)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = maxSets;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

	VK_CHECK_MESSAGE(vkCreateDescriptorPool(device->Value(), &poolInfo, nullptr, &descriptorPool), "Failed to create descriptor pool!");
}

void Vk::DescriptorPool::Cleanup()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if (descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device->Value(), descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;
	}
}

Vk::DescriptorPoolBuilder::~DescriptorPoolBuilder()
{
	Reset();
}

Vk::DescriptorPoolBuilder& Vk::DescriptorPoolBuilder::Reset()
{
	maxSets = 0;
	poolSizes.clear();

	return *this;
}

Vk::DescriptorPoolBuilder& Vk::DescriptorPoolBuilder::SetPoolSize(VkDescriptorType type, uint32_t size)
{
	poolSizes[type] = size;
	return *this;
}

Vk::DescriptorPoolBuilder& Vk::DescriptorPoolBuilder::SetMaxSets(uint32_t size)
{
	maxSets = size;
	return *this;
}

std::shared_ptr<Vk::DescriptorPool> Vk::DescriptorPoolBuilder::BuildDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> poolSizesVec;
	poolSizesVec.reserve(poolSizes.size());

	for (auto& [type, size] : poolSizes)
		poolSizesVec.push_back(VkDescriptorPoolSize(type, size));

	return std::make_shared<DescriptorPool>(poolSizesVec, maxSets);
}