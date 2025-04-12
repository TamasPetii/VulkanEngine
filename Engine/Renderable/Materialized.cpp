#include "Materialized.h"

std::shared_ptr<Vk::Buffer> Materialized::GetMaterialBuffer()
{
    return materialBuffer;
}

std::shared_ptr<Vk::Buffer> Materialized::GetMaterialIndexBuffer()
{
    return materialIndexBuffer;
}

void Materialized::UploadMaterialDataToGpu()
{
    //No materials found, or at least 1 mesh had no materials
    if (materials.empty() || std::find_if(materialIndices.begin(), materialIndices.end(), [&](uint32_t index) -> bool { return index == UINT32_MAX; }) != materialIndices.end())
    {
        MaterialComponent material;
        material.color = glm::vec4(10, 0, 0, 1);

        uint32_t defaultIndex = materials.size();
        materials.push_back(material);

        for (auto& materialIndex : materialIndices)
        {
            if (materialIndex == UINT32_MAX)
                materialIndex = defaultIndex;
        }
    }

    //Upload the materials datas to GPU
    {
        std::vector<MaterialComponentGPU> materialsGPU(materials.begin(), materials.end());
        VkDeviceSize bufferSize = sizeof(MaterialComponentGPU) * materialsGPU.size();

        Vk::BufferConfig stagingConfig;
        stagingConfig.size = bufferSize;
        stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
        stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        Vk::Buffer stagingBuffer{ stagingConfig };

        void* mappedBuffer = stagingBuffer.MapMemory();
        memcpy(mappedBuffer, materialsGPU.data(), (size_t)bufferSize);
        stagingBuffer.UnmapMemory();

        Vk::BufferConfig config;
        config.size = bufferSize;
        config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
        config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        materialBuffer = std::make_shared<Vk::Buffer>(config);

        Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
            [&](VkCommandBuffer commandBuffer) -> void {
                Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), materialBuffer->Value(), bufferSize);
            }
        );
    }

    //Upload the material indices to gpu
    {
        VkDeviceSize bufferSize = sizeof(uint32_t) * materialIndices.size();

        Vk::BufferConfig stagingConfig;
        stagingConfig.size = bufferSize;
        stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
        stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        Vk::Buffer stagingBuffer{ stagingConfig };

        void* mappedBuffer = stagingBuffer.MapMemory();
        memcpy(mappedBuffer, materialIndices.data(), (size_t)bufferSize);
        stagingBuffer.UnmapMemory();

        Vk::BufferConfig config;
        config.size = bufferSize;
        config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
        config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        materialIndexBuffer = std::make_shared<Vk::Buffer>(config);

        Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
            [&](VkCommandBuffer commandBuffer) -> void {
                Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), materialIndexBuffer->Value(), bufferSize);
            }
        );
    }
}
