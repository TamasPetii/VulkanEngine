#pragma once
#include <memory>
#include <set>
#include <string>
#include <future>
#include <thread>
#include <unordered_map>

#include "Engine/Renderable/Model/Model.h"

class ENGINE_API ModelManager
{
public:
	ModelManager(std::shared_ptr<ImageManager> imageManager);
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	std::shared_ptr<Model> LoadModel(const std::string& path);
	std::shared_ptr<Model> GetModel(const std::string& path);
	const auto& GetModels() { return models; }
	void Update();
private:
	std::shared_ptr<ImageManager> imageManager = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Model>> models;
	std::unordered_map<std::string, std::future<void>> futures;
};

