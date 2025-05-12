#pragma once
#include "Engine/EngineApi.h"
#include "Engine/Utils/BatchUploaded.h"
#include "Engine/Utils/AsyncLoaded.h"

#include <span>
#include <memory>
#include <vector>
#include <future>
#include <thread>
#include <concepts>
#include <unordered_set>
#include <functional>

template<typename T>
concept IsAsyncBatchUploadedItem = std::derived_from<T, BatchUploaded> && std::derived_from<T, AsyncLoaded>;

template <IsAsyncBatchUploadedItem ItemType, uint32_t minBatchSize>
class AsyncBatchedGpuUploadedManager 
{
public:
public:
    AsyncBatchedGpuUploadedManager() = default;
    AsyncBatchedGpuUploadedManager(const AsyncBatchedGpuUploadedManager&) = delete;
    AsyncBatchedGpuUploadedManager& operator=(const AsyncBatchedGpuUploadedManager&) = delete;
protected:
    void TryToSubmitBatch(bool noMoreItem);
    std::vector<ItemType*> SubmitBatch(std::vector<ItemType*> batch);
    std::vector<ItemType*> CompleteFinishedFutures();
protected:
    std::vector<ItemType*> batch;
    std::vector<std::future<std::vector<ItemType*>>> batchFutures;
};

template<IsAsyncBatchUploadedItem ItemType, uint32_t minBatchSize>
inline std::vector<ItemType*> AsyncBatchedGpuUploadedManager<ItemType, minBatchSize>::SubmitBatch(std::vector<ItemType*> batch) //Need to copy batch -> It gets cleared
{
    std::vector<VkCommandBufferSubmitInfo> submitInfos;
    submitInfos.reserve(batch.size());

    for (auto item : batch)
        submitInfos.push_back(static_cast<BatchUploaded*>(item)->GetCommandBufferSubmitInfo());

    Vk::VulkanContext::GetContext()->GetImmediateQueue()->SubmitGraphics(submitInfos);

    for (auto item : batch)
    {
        static_cast<BatchUploaded*>(item)->DestoryCommandPoolAndBuffer();
        static_cast<AsyncLoaded*>(item)->state = LoadState::GpuUploaded;
    }

    return batch;
}

template<IsAsyncBatchUploadedItem ItemType, uint32_t minBatchSize>
inline std::vector<ItemType*> AsyncBatchedGpuUploadedManager<ItemType, minBatchSize>:: CompleteFinishedFutures()
{
    std::vector<ItemType*> completedFutures;
    for (auto it = batchFutures.begin(); it != batchFutures.end();) {
        auto& future = *it;
        if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                auto finishedItems = future.get();

                completedFutures.insert(completedFutures.end(), finishedItems.begin(), finishedItems.end());

                it = batchFutures.erase(it);
            }
            catch (const std::exception& e) {
                it = batchFutures.erase(it);
            }
        }
        else {
            ++it;
        }
    }

    return completedFutures;
}

template<IsAsyncBatchUploadedItem ItemType, uint32_t minBatchSize>
inline void AsyncBatchedGpuUploadedManager<ItemType, minBatchSize>::TryToSubmitBatch(bool noMoreItem)
{
    if (batch.size() >= minBatchSize || (!batch.empty() && noMoreItem))
    {
        batchFutures.push_back(std::async(std::launch::async, &AsyncBatchedGpuUploadedManager<ItemType, minBatchSize>::SubmitBatch, this, batch));
        batch.clear();
    }
}