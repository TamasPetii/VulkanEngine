#include "DescriptorPool.h"


Vk::DescriptorPool::DescriptorPool(std::span<VkDescriptorPoolSize> poolSizes, uint32_t setSize)
{
	Initialize(poolSizes, setSize);
}

Vk::DescriptorPool::~DescriptorPool()
{
	Cleanup();
}

const VkDescriptorPool& Vk::DescriptorPool::Value() const
{
	return descriptorPool;
}

void Vk::DescriptorPool::Initialize(std::span<VkDescriptorPoolSize> poolSizes, uint32_t setSize)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = setSize;

	VK_CHECK_MESSAGE(vkCreateDescriptorPool(device->Value(), &poolInfo, nullptr, &descriptorPool), "Failed to create descriptor pool!");
}

void Vk::DescriptorPool::Cleanup()
{
	auto device = VulkanContext::GetContext()->GetDevice();
	vkDestroyDescriptorPool(device->Value(), descriptorPool, nullptr);
}

Vk::DescriptorPoolBuilder::~DescriptorPoolBuilder()
{
	Reset();
}

Vk::DescriptorPoolBuilder& Vk::DescriptorPoolBuilder::Reset()
{
	setSize = 0;
	poolSizes.clear();
}

Vk::DescriptorPoolBuilder& Vk::DescriptorPoolBuilder::SetPoolSize(VkDescriptorType type, uint32_t size)
{
	poolSizes[type] = size;
}

Vk::DescriptorPoolBuilder& Vk::DescriptorPoolBuilder::SetSetSize(uint32_t size)
{
	setSize = size;
}

std::shared_ptr<Vk::DescriptorPool> Vk::DescriptorPoolBuilder::BuildDescriptorPool()
{
	std::vector<VkDescriptorPoolSize> poolSizesVec;
	poolSizesVec.reserve(poolSizes.size());

	for (auto& [type, size] : poolSizes)
		poolSizesVec.push_back(VkDescriptorPoolSize(type, size));

	return std::make_shared<DescriptorPool>(poolSizesVec, setSize);
}