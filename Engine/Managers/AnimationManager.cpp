#include "AnimationManager.h"

std::shared_ptr<Animation> AnimationManager::LoadAnimation(const std::string& path)
{
    std::lock_guard<std::mutex> lock(asyncMutex);

    if (animations.find(path) != animations.end())
        return animations.at(path);

    log << std::format("[Animation Thread Started] : {}", path) << "\n";

    std::shared_ptr<Animation> animation = std::make_shared<Animation>(GetAvailableIndex());
    animations[path] = animation;

    futures.emplace(path, std::async(std::launch::async, &Animation::Load, animations.at(path), path));

    return animations.at(path);
}

std::shared_ptr<Animation> AnimationManager::GetAnimation(const std::string& path)
{
    std::lock_guard<std::mutex> lock(asyncMutex);

    if (animations.find(path) == animations.end())
        return nullptr;

    return animations.at(path);
}

AnimationManager::AnimationManager()
{
}

AnimationManager::~AnimationManager()
{
    animations.clear();
}

void AnimationManager::Update()
{
    std::lock_guard<std::mutex> lock(asyncMutex);

    auto completedFutures = AsyncManager::CompleteFinishedFutures();

    for (auto path : completedFutures)
    {
        log << std::format("[Animation Thread Finished] : {}", path) << "\n";

        auto animation = animations.at(path);
        if (animation && animation->state == LoadState::GpuUploaded)
        {
            static_cast<VkDeviceAddress*>(deviceAddresses->GetHandler())[animation->GetAddressArrayIndex()] = animation->GetVertexBoneBuffer()->GetAddress();
            animation->state = LoadState::Ready; 

            log << std::format("Animation Vertex Bone Buffer Uploaded: {} ", animation->GetAddressArrayIndex()) << "\n";
        }
    }
}
