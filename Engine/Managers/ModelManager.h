#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "Engine/Renderable/Model/Model.h"

class ENGINE_API ModelManager
{
public:
	ModelManager(std::shared_ptr<ImageManager> imageManager);
	std::shared_ptr<Model> LoadModel(const std::string& path);
	std::shared_ptr<Model> GetModel(const std::string& path);
	const auto& GetModels() { return models; }
private:
	std::shared_ptr<ImageManager> imageManager = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Model>> models;
};

