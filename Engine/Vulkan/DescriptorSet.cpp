#include "DescriptorSet.h"


VkDescriptorSetLayoutBinding Vk::DescriptorSet::BuildLayoutBindingInfo(uint32_t binding, VkDescriptorType type, VkShaderStageFlags flags)
{
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = type;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = flags;
	layoutBinding.pImmutableSamplers = nullptr;

	return layoutBinding;
}

Vk::DescriptorSet::DescriptorSet(VkDescriptorPool pool, std::span<DescriptorLayoutBuffer> bufferLayouts, std::span<DescriptorLayoutImage> imageLayouts)
{
	Initialize(pool, bufferLayouts, imageLayouts);
}

Vk::DescriptorSet::~DescriptorSet()
{
	Cleanup();
}

void Vk::DescriptorSet::Free(VkDescriptorPool pool)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if (descriptorSet != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(device->Value(), pool, 1, &descriptorSet);
		descriptorSet = VK_NULL_HANDLE;

		Cleanup();
	}
}

const VkDescriptorSet& Vk::DescriptorSet::Value() const
{
	return descriptorSet;
}

const VkDescriptorSetLayout& Vk::DescriptorSet::Layout() const
{
	return descriptorlayout;
}

void Vk::DescriptorSet::Initialize(VkDescriptorPool pool, std::span<DescriptorLayoutBuffer> bufferLayouts, std::span<DescriptorLayoutImage> imageLayouts)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	for (auto& bufferLayout : bufferLayouts)
	{
		VkDescriptorSetLayoutBinding layoutBinding = BuildLayoutBindingInfo(bufferLayout.config.binding, bufferLayout.config.type, bufferLayout.config.flags);
		layoutBindings.push_back(layoutBinding);
	}

	for (auto& imageLayout : imageLayouts)
	{
		VkDescriptorSetLayoutBinding layoutBinding = BuildLayoutBindingInfo(imageLayout.config.binding, imageLayout.config.type, imageLayout.config.flags);
		layoutBindings.push_back(layoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();

	VK_CHECK_MESSAGE(vkCreateDescriptorSetLayout(device->Value(), &layoutInfo, nullptr, &descriptorlayout), "Failed to create descriptor set layout!");

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorlayout;

	VK_CHECK_MESSAGE(vkAllocateDescriptorSets(device->Value(), &allocInfo, &descriptorSet), "Failed to allocate descriptor sets!");

	std::vector<VkWriteDescriptorSet> descriptorWrites{};

	for (auto& bufferLayout : bufferLayouts)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = bufferLayout.buffer;
		bufferInfo.offset = bufferLayout.offset;
		bufferInfo.range = bufferLayout.range;

		VkWriteDescriptorSet writeInfo{};
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet = descriptorSet;
		writeInfo.dstBinding = bufferLayout.config.binding;
		writeInfo.dstArrayElement = 0;
		writeInfo.descriptorType = bufferLayout.config.type;
		writeInfo.descriptorCount = 1;
		writeInfo.pBufferInfo = &bufferInfo;

		descriptorWrites.push_back(writeInfo);
	}

	for (auto& imageLayout : imageLayouts)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = imageLayout.image;
		imageInfo.sampler = imageLayout.sampler;
		imageInfo.imageLayout = imageLayout.layout;

		VkWriteDescriptorSet writeInfo{};
		writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet = descriptorSet;
		writeInfo.dstBinding = imageLayout.config.binding;
		writeInfo.dstArrayElement = 0;
		writeInfo.descriptorType = imageLayout.config.type;
		writeInfo.descriptorCount = 1;
		writeInfo.pImageInfo = &imageInfo;

		descriptorWrites.push_back(writeInfo);
	}

	vkUpdateDescriptorSets(device->Value(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Vk::DescriptorSet::Cleanup()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if (descriptorlayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device->Value(), descriptorlayout, nullptr);
		descriptorlayout = VK_NULL_HANDLE;
	}
}

Vk::DescriptorSetBuilder& Vk::DescriptorSetBuilder::AddDescriptorLayoutBuffer(uint32_t binding, VkDescriptorType type, VkShaderStageFlags flags, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
	DescriptorLayoutBuffer bufferLayout{};
	bufferLayout.config.binding = binding;
	bufferLayout.config.type = type;
	bufferLayout.config.flags = flags;
	bufferLayout.buffer = buffer;
	bufferLayout.offset = offset;
	bufferLayout.range = range;

	bufferLayouts.push_back(bufferLayout);

	return *this;
}

Vk::DescriptorSetBuilder& Vk::DescriptorSetBuilder::AddDescriptorLayoutImage(uint32_t binding, VkDescriptorType type, VkShaderStageFlags flags, VkImageView image, VkSampler sampler, VkImageLayout layout)
{
	DescriptorLayoutImage imageLayout{};
	imageLayout.config.binding = binding;
	imageLayout.config.type = type;
	imageLayout.config.flags = flags;
	imageLayout.image = image;
	imageLayout.sampler = sampler;
	imageLayout.layout = layout;

	imageLayouts.push_back(imageLayout);

	return *this;
}

std::shared_ptr<Vk::DescriptorSet> Vk::DescriptorSetBuilder::BuildDescriptorSet(VkDescriptorPool pool)
{
	return std::make_shared<Vk::DescriptorSet>(pool, bufferLayouts, imageLayouts);
}
