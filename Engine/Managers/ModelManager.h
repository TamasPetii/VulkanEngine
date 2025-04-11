#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "Engine/Renderable/Model/Model.h"

class ENGINE_API ModelManager
{
public:
	std::shared_ptr<Model> LoadModel(const std::string& path);
	std::shared_ptr<Model> GetModel(const std::string& path);
private:
	std::unordered_map<std::string, std::shared_ptr<Model>> models;
};

