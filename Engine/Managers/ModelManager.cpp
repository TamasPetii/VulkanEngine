#include "ModelManager.h"

ModelManager::ModelManager(std::shared_ptr<ImageManager> imageManager) : 
    imageManager(imageManager)
{
}

std::shared_ptr<Model> ModelManager::LoadModel(const std::string& path)
{
    if (models.find(path) != models.end())
        return models.at(path);

    std::shared_ptr<Model> model = std::make_shared<Model>(imageManager);

    if (!model->Load(path))
        return nullptr;

    models[path] = model;
    return models.at(path);
}

std::shared_ptr<Model> ModelManager::GetModel(const std::string& path)
{
    if (models.find(path) == models.end())
        return nullptr;

    return models.at(path);
}
