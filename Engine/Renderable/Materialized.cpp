#include "Materialized.h"
#include <algorithm>
#include <execution>
#include <ranges>

std::shared_ptr<Vk::Buffer> Materialized::GetMaterialBuffer()
{
    return materialBuffer;
}

void Materialized::UploadMaterialDataToGpu()
{
    //No materials found, or at least 1 mesh had no materials
    if (materials.empty())
    {
        MaterialComponent material;
        material.color = glm::vec4(10, 0, 0, 1);
        materials.push_back(material);
    }
    //TODO: Is it even handled?

    {
        VkDeviceSize bufferSize = sizeof(MaterialComponentGPU) * materials.size();

        Vk::BufferConfig config;
        config.size = bufferSize;
        config.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
        config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        materialBuffer = std::make_shared<Vk::Buffer>(config);
        materialBuffer->MapMemory();
        auto materialBufferHandler = static_cast<MaterialComponentGPU*>(materialBuffer->GetHandler());

        auto vertex_index_range = std::views::iota(0u, (uint32_t)materials.size());
        std::for_each(std::execution::par, vertex_index_range.begin(), vertex_index_range.end(),
        [&](auto materialIndex) -> void {
            bool albedoReady = false, normalReady = false, metallicReady = false, roughnessReady = false;     
            auto& material = materials[materialIndex];

            do
            {
                albedoReady = !material.albedo || (material.albedo->state == LoadState::Ready || material.albedo->state == LoadState::Failed);
                normalReady = !material.normal || (material.normal->state == LoadState::Ready || material.normal->state == LoadState::Failed);
                metallicReady = !material.metallic || (material.metallic->state == LoadState::Ready || material.metallic->state == LoadState::Failed);
                roughnessReady = !material.roughness || (material.roughness->state == LoadState::Ready || material.roughness->state == LoadState::Failed);
            } while (!albedoReady || !normalReady || !metallicReady || !roughnessReady);

            materialBufferHandler[materialIndex] = MaterialComponentGPU(material);
        });
    }
}
