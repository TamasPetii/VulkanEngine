#pragma once
#include "Engine/EngineApi.h"
#include "Engine/Utils/BatchUploaded.h"
#include "Engine/Utils/AsyncLoaded.h"

#include <span>
#include <memory>
#include <vector>
#include <future>
#include <concepts>
#include <unordered_set>

template<typename T>
concept IsAsyncBatchUploadedItem = std::derived_from<T, BatchUploaded> && std::derived_from<T, AsyncLoaded>;

template <IsAsyncBatchUploadedItem ItemType, uint32_t batchSize>
class ENGINE_API AsyncBatchedGpuUploadedManager : public 
{
public:
    AsyncBatchedGpuUploadedManager() = default;
    AsyncBatchedGpuUploadedManager(const AsyncBatchedGpuUploadedManager&) = delete;
    AsyncBatchedGpuUploadedManager& operator=(const AsyncBatchedGpuUploadedManager&) = delete;
protected:
    std::vector<ItemType> CompleteFinishedFutures();
    void UploadBatchDataToGpu();
    void TryToSendBatchToGpu();
protected:
	std::vector<ItemType> batch;
	std::unordered_set<std::unique_ptr<std::future<void>>> batchFutures;
};



template<IsAsyncBatchUploadedItem ItemType, uint32_t batchSize>
inline void AsyncBatchedGpuUploadedManager<ItemType, batchSize>::UploadBatchDataToGpu()
{
    std::vector<VkCommandBufferSubmitInfo> submitInfos;
    submitInfos.reserve(batch.size());

    for (auto& item : batch)
        submitInfos.push_back(static_cast<BatchUploaded*>(&item)->GetCommandBufferSubmitInfo());

    Vk::VulkanContext::GetContext()->GetImmediateQueue()->SubmitGraphics(submitInfos);

    for (auto& item : batch)
    {
        static_cast<BatchUploaded*>(item)->DestoryCommandPoolAndBuffer();
        static_cast<AsyncLoaded*>(item)->state = LoadState::GpuUploaded;
    }
}
template<IsAsyncBatchUploadedItem ItemType, uint32_t batchSize>
inline void AsyncBatchedGpuUploadedManager<ItemType, batchSize>::TryToSendBatchToGpu()
{
    if (batch.size() >= batchSize || (!batch.empty() && imageLoadFutures.empty()))
    {
        std::cout << "Images Uploaded with batch size: " << imagesToUploadGpu.size() << "\n";
        imagesToUploadGpuFutures.insert(std::make_unique<std::future<void>>(std::async(std::launch::async, &ImageManager::UploadBatchedImages, this, imagesToUploadGpu)));
        imagesToUploadGpu.clear();
    }
}