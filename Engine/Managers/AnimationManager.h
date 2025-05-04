#pragma once
#include <memory>
#include <set>
#include <string>
#include <future>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "DescriptorArrayIndexed.h"
#include "Engine/Animation/Animation.h"

class ENGINE_API AnimationManager : public DescriptorArrayIndexed
{
public:
	AnimationManager() = default;
	AnimationManager(const AnimationManager&) = delete;
	AnimationManager& operator=(const AnimationManager&) = delete;

	void Update();
	std::shared_ptr<Animation> LoadAnimation(const std::string& path);
	std::shared_ptr<Animation> GetAnimation(const std::string& path);
	const auto& GetAnimations() { return animations; }
private:
	std::mutex loadMutex;
	std::unordered_map<std::string, std::shared_ptr<Animation>> animations;
	std::unordered_map<std::string, std::future<void>> futures;
};

