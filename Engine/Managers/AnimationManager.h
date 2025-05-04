#pragma once
#include <memory>
#include <set>
#include <string>
#include <future>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "Engine/Animation/Animation.h"

class ENGINE_API AnimationManager
{
public:
	AnimationManager() = default;
	AnimationManager(const AnimationManager&) = delete;
	AnimationManager& operator=(const AnimationManager&) = delete;

	void Update();
	std::shared_ptr<Animation> LoadAnimation(const std::string& path);
	std::shared_ptr<Animation> GetAnimation(const std::string& path);
	const auto& GetAnimatios() { return animations; }
private:
	std::mutex loadMutex;
	std::unordered_map<std::string, std::shared_ptr<Animation>> animations;
	std::unordered_map<std::string, std::future<void>> futures;
};

