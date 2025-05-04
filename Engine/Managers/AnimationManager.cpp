#include "AnimationManager.h"

std::shared_ptr<Animation> AnimationManager::LoadAnimation(const std::string& path)
{
    std::unique_lock<std::mutex> lock(loadMutex);

    if (animations.find(path) != animations.end())
        return animations.at(path);

    std::shared_ptr<Animation> animation = std::make_shared<Animation>();
    animations[path] = animation;

    futures.emplace(path, std::async(std::launch::async, &Animation::Load, animations.at(path), path));

    return animations.at(path);
}

std::shared_ptr<Animation> AnimationManager::GetAnimation(const std::string& path)
{
    if (animations.find(path) == animations.end())
        return nullptr;

    return animations.at(path);
}

void AnimationManager::Update()
{
    //Need the lock, might delete future from futures map while LoadModel inserts future in it.
    std::unique_lock<std::mutex> lock(loadMutex);

    for (auto it = futures.begin(); it != futures.end();) {
        if (it->second.valid() && it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                it->second.get();
                it = futures.erase(it);
                std::cout << "Async animation loading thread finished successfuly" << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Async animation loading thread error: " << e.what() << std::endl;
                it = futures.erase(it);
            }
        }
        else {
            ++it;
        }
    }

    for (auto& [path, animation] : animations)
    {
        if (futures.find(path) == futures.end() && animation->state == LoadState::GpuUploaded)
            animation->state = LoadState::Ready;
    }
}
