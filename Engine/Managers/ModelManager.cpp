#include "ModelManager.h"

std::shared_ptr<Model> ModelManager::LoadModel(const std::string& path)
{
    if (models.find(path) != models.end())
        return models.at(path);

    std::shared_ptr<Model> model = std::make_shared<Model>();

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
